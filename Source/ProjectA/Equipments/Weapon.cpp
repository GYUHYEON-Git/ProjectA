// Fill out your copyright notice in the Description page of Project Settings.


#include "Equipments/Weapon.h"

#include "MyGameplayTags.h"
#include "Characters/PlayerCharacter.h"
#include "Animations/MyAnimInstance.h"
#include "Data/MontageActionData.h"
#include "Components/CombatComponent.h"
#include "Components/WeaponCollisionComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Shield.h"

AWeapon::AWeapon() {
	WeaponCollision = CreateDefaultSubobject<UWeaponCollisionComponent>("MainCollision");
	WeaponCollision->OnHitActor.AddUObject(this, &ThisClass::OnHitActor);

	SecondWeaponCollision = CreateDefaultSubobject<UWeaponCollisionComponent>("SecondCollision");
	SecondWeaponCollision->OnHitActor.AddUObject(this, &ThisClass::OnHitActor);

	StaminaCostMap.Add(MyGameplayTags::Character_Attack_Light, 7.f);
	StaminaCostMap.Add(MyGameplayTags::Character_Attack_Running, 12.f);
	StaminaCostMap.Add(MyGameplayTags::Character_Attack_Special, 15.f);
	StaminaCostMap.Add(MyGameplayTags::Character_Attack_Heavy, 20.f);

	DamageMultiplierMap.Add(MyGameplayTags::Character_Attack_Heavy, 1.8f);
	DamageMultiplierMap.Add(MyGameplayTags::Character_Attack_Running, 1.8f);
	DamageMultiplierMap.Add(MyGameplayTags::Character_Attack_Special, 2.1f);
}

void AWeapon::EquipItem() {
	Super::EquipItem();
	CombatComponent = GetOwner()->GetComponentByClass<UCombatComponent>();
	if (CombatComponent) {
		CombatComponent->SetWeapon(this);
		// Selects the equipment socket based on the combat state.
		const FName AttackSocket = CombatComponent->IsCombatEnabled() ? EquipSocketName : UnequipSocketName;
		AttachToOwner(AttackSocket);
		WeaponCollision->SetWeaponMesh(Mesh);
		// Updates the CombatType based on the equipped weapon. -> Changes the Blend Space.
		if (ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner())) {
			UAnimInstance* BaseAnim = OwnerCharacter->GetMesh()->GetAnimInstance();
			if (UMyAnimInstance* Anim = Cast<UMyAnimInstance>(OwnerCharacter->GetMesh()->GetAnimInstance())) {
				Anim->UpdateCombatMode(CombatType);
			}
		}
		WeaponCollision->AddIgnoredActor(GetOwner());
		// If a shield is equipped, selects its equip socket based on the combat state and CombatType.
		if (AShield* Shield = CombatComponent->GetShield()) {
			FName ShieldAttachSocket = Shield->GetUnequipSocketName();
			if (CombatType == ECombatType::SwordShield) {
				if (CombatComponent->IsCombatEnabled()) {
					ShieldAttachSocket = Shield->GetEquipSocketName();
				}
			}
			Shield->AttachToOwner(ShieldAttachSocket);
		}
	}
}

float AWeapon::GetStaminaCost(const FGameplayTag& InTag) const {
	if (StaminaCostMap.Contains(InTag)) {
		return StaminaCostMap[InTag];
	}
	return 0.0f;
}

UAnimMontage* AWeapon::GetMontageForTag(const FGameplayTag& Tag, const int32 Index) const {
	return MontageActionData->GetMontageForTag(Tag, Index);
}

UAnimMontage* AWeapon::GetRandomMontageForTag(const FGameplayTag& Tag) const {
	return MontageActionData->GetRandomMontageForTag(Tag);
}

UAnimMontage* AWeapon::GetHitReactMontage(const AActor* Attacker) const {
	// Calculate the LookAt rotation (rotation that makes the current actor face the attacker)
	const FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetOwner()->GetActorLocation(), Attacker->GetActorLocation());
	// Calculate the difference between the current actor's rotation and the LookAt rotation
	const FRotator DeltaRotation = UKismetMathLibrary::NormalizedDeltaRotator(GetOwner()->GetActorRotation(), LookAtRotation);
	// Use only the rotation difference around the Z-axis
	const float DeltaZ = DeltaRotation.Yaw;

	EHitDirection HitDirection = EHitDirection::Front;

	// Sets the hit direction based on the angle to the attacker.
	if (UKismetMathLibrary::InRange_FloatFloat(DeltaZ, -45.f, 45.f)) {
		HitDirection = EHitDirection::Front;
	}
	else if (UKismetMathLibrary::InRange_FloatFloat(DeltaZ, 45.f, 135.f)) {
		HitDirection = EHitDirection::Left;
	}
	else if (UKismetMathLibrary::InRange_FloatFloat(DeltaZ, 135.f, 180.f)
		|| UKismetMathLibrary::InRange_FloatFloat(DeltaZ, -180.f, -135.f)) {
		HitDirection = EHitDirection::Back;
	}
	else if (UKismetMathLibrary::InRange_FloatFloat(DeltaZ, -135.f, -45.f)) {
		HitDirection = EHitDirection::Right;
	}

	// Plays the hit animation based on the hit direction.
	UAnimMontage* SelectedMontage = nullptr;
	switch (HitDirection) {
	case EHitDirection::Front:
		SelectedMontage = GetMontageForTag(MyGameplayTags::Character_Action_HitReaction, 0);
		break;
	case EHitDirection::Back:
		SelectedMontage = GetMontageForTag(MyGameplayTags::Character_Action_HitReaction, 1);
		break;
	case EHitDirection::Left:
		SelectedMontage = GetMontageForTag(MyGameplayTags::Character_Action_HitReaction, 2);
		break;
	case EHitDirection::Right:
		SelectedMontage = GetMontageForTag(MyGameplayTags::Character_Action_HitReaction, 3);
		break;
	}

	return SelectedMontage;
}

bool AWeapon::HasValidMontage(const FGameplayTag& Tag) const {
	return MontageActionData->HasValidMontage(Tag);
}

float AWeapon::GetAttackDamage() const {
	if (const AActor* OwnerActor = GetOwner()) {
		const FGameplayTag LastAttackType = CombatComponent->GetLastAttackType();
		if (DamageMultiplierMap.Contains(LastAttackType)) {
			const float Multiplier = DamageMultiplierMap[LastAttackType];
			return BaseDamage * Multiplier;
		}
	}
	return BaseDamage;
}

// Trigger function for enabling attack collision during an AnimNotify State.
void AWeapon::ActivateCollision(EWeaponCollisionType InCollisionType) {
	switch (InCollisionType) {
	case EWeaponCollisionType::MainCollision:
		WeaponCollision->TurnOnCollision();
		break;
	case EWeaponCollisionType::SecondCollision:
		SecondWeaponCollision->TurnOnCollision();
		break;
	}
}

// Trigger function for enabling attack collision during an AnimNotify State.
void AWeapon::DeactivateCollision(EWeaponCollisionType InCollisionType) {
	switch (InCollisionType) {
	case EWeaponCollisionType::MainCollision:
		WeaponCollision->TurnOffCollision();
		break;
	case EWeaponCollisionType::SecondCollision:
		SecondWeaponCollision->TurnOffCollision();
		break;
	}
}

void AWeapon::OnHitActor(const FHitResult& Hit) {
	AActor* TargetActor = Hit.GetActor();
	FVector DamageDirection = GetOwner()->GetActorForwardVector();
	float AttackDamage = GetAttackDamage();
	UGameplayStatics::ApplyPointDamage(
		TargetActor,
		AttackDamage, 
		DamageDirection,
		Hit,
		GetOwner()->GetInstigatorController(),
		this,
		nullptr);
}