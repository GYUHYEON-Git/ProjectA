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
		const FName AttackSocket = CombatComponent->IsCombatEnabled() ? EquipSocketName : UnequipSocketName;
		AttachToOwner(AttackSocket);
		WeaponCollision->SetWeaponMesh(Mesh);
		if (ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner())) {
			UAnimInstance* BaseAnim = OwnerCharacter->GetMesh()->GetAnimInstance();
			if (UMyAnimInstance* Anim = Cast<UMyAnimInstance>(OwnerCharacter->GetMesh()->GetAnimInstance())) {
				Anim->UpdateCombatMode(CombatType);
			}
		}
		WeaponCollision->AddIgnoredActor(GetOwner());
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
	// LookAt 회전값을 구합니다. (현재 Actor가 공격자를 바라보는 회전값)
	const FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetOwner()->GetActorLocation(), Attacker->GetActorLocation());
	// 현재 Actor의 회전값과 LookAt 회전값의 차이를 구합니다.
	const FRotator DeltaRotation = UKismetMathLibrary::NormalizedDeltaRotator(GetOwner()->GetActorRotation(), LookAtRotation);
	// Z축 기준의 회전값 차이만을 취합니다.
	const float DeltaZ = DeltaRotation.Yaw;

	EHitDirection HitDirection = EHitDirection::Front;

	if (UKismetMathLibrary::InRange_FloatFloat(DeltaZ, -45.f, 45.f)) {
		HitDirection = EHitDirection::Front;
		UE_LOG(LogTemp, Log, TEXT("Front"));
	}
	else if (UKismetMathLibrary::InRange_FloatFloat(DeltaZ, 45.f, 135.f)) {
		HitDirection = EHitDirection::Left;
		UE_LOG(LogTemp, Log, TEXT("Left"));
	}
	else if (UKismetMathLibrary::InRange_FloatFloat(DeltaZ, 135.f, 180.f)
		|| UKismetMathLibrary::InRange_FloatFloat(DeltaZ, -180.f, -135.f)) {
		HitDirection = EHitDirection::Back;
		UE_LOG(LogTemp, Log, TEXT("Back"));
	}
	else if (UKismetMathLibrary::InRange_FloatFloat(DeltaZ, -135.f, -45.f)) {
		HitDirection = EHitDirection::Right;
		UE_LOG(LogTemp, Log, TEXT("Right"));
	}

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
	// Damage Direction
	FVector DamageDirection = GetOwner()->GetActorForwardVector();
	// Attack Damage
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