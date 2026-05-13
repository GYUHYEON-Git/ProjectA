// Fill out your copyright notice in the Description page of Project Settings.


#include "Equipments/Weapon.h"

#include "MyGameplayTags.h"
#include "Data/MontageActionData.h"
#include "Components/CombatComponent.h"

AWeapon::AWeapon() {
	StaminaCostMap.Add(MyGameplayTags::Character_Attack_Light, 10.f);
	StaminaCostMap.Add(MyGameplayTags::Character_Attack_Heavy, 20.f);
}

void AWeapon::EquipItem() {
	Super::EquipItem();
	CombatComponent = GetOwner()->GetComponentByClass<UCombatComponent>();
	if (CombatComponent) {
		CombatComponent->SetWeapon(this);
		const FName AttackSocket = CombatComponent->IsCombatEnabled() ? EquipSocketName : UnequipSocketName;
		AttachToOwner(AttackSocket);
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
