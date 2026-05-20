// Fill out your copyright notice in the Description page of Project Settings.


#include "Equipments/Weapon.h"

#include "MyGameplayTags.h"
#include "Characters/PlayerCharacter.h"
#include "Animations/MyAnimInstance.h"
#include "Data/MontageActionData.h"
#include "Components/CombatComponent.h"
#include "Components/WeaponCollisionComponent.h"
#include "Kismet/GameplayStatics.h"

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
		if (APlayerCharacter* OwnerCharacter = Cast<APlayerCharacter>(GetOwner())) {
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