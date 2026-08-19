// Fill out your copyright notice in the Description page of Project Settings.


#include "Equipments/Shield.h"
#include "Components/CombatComponent.h"
#include "Weapon.h"

void AShield::EquipItem() {
	Super::EquipItem();
	if (const AActor* OwnActor = GetOwner()) {
		if (UCombatComponent* CombatComponent = OwnActor->GetComponentByClass<UCombatComponent>()) {
			CombatComponent->SetShield(this);

			FName AttachSocketName = UnequipSocketName;

			// Selects the shield equip socket based on whether a weapon is equipped, the CombatType, and the combat state.
			const AWeapon* MainWeapon = CombatComponent->GetMainWeapon();
			if (IsValid(MainWeapon)) {
				const ECombatType CombatType = MainWeapon->GetCombatType();
				if (CombatType == ECombatType::SwordShield) {
					if (CombatComponent->IsCombatEnabled()) {
						AttachSocketName = EquipSocketName;
					}
				}
			}
			AttachToOwner(AttachSocketName);
		}
	}
}
