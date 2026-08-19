// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/AnimNotify_EquipWeapon.h"

#include "MyGameplayTags.h"
#include "Components/CombatComponent.h"
#include "Equipments/Weapon.h"
#include "Equipments/Shield.h"
#include "Define.h"

UAnimNotify_EquipWeapon::UAnimNotify_EquipWeapon(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer) {
}

// Changes the equipment socket at a specific frame using an AnimNotify.
void UAnimNotify_EquipWeapon::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) {
	Super::Notify(MeshComp, Animation, EventReference);
	if (const AActor* Owner = MeshComp->GetOwner()) {
		if (UCombatComponent* CombatComponent = Owner->GetComponentByClass<UCombatComponent>()) {
			if (AWeapon* MainWeapon = CombatComponent->GetMainWeapon()) {
				// If the equipment being equipped or unequipped is a weapon.
				if (EquipmentType == EEquipmentType::Weapon) {
					bool bCombatEnable = CombatComponent->IsCombatEnabled();
					FName WeaponSocketName;
					// For the equip animation, sets the socket name to EquipSocket and enters combat state.
					if (MontageActionTag == MyGameplayTags::Character_Action_Equip) {
						bCombatEnable = true;
						WeaponSocketName = MainWeapon->GetEquipSocketName();
					}
					// For the unequip animation, sets the socket name to UnequipSocket and exits combat state.
					else if (MontageActionTag == MyGameplayTags::Character_Action_Unequip) {
						bCombatEnable = false;
						WeaponSocketName = MainWeapon->GetUnequipSocketName();
					}
					CombatComponent->SetCombatEnabled(bCombatEnable);
					MainWeapon->AttachToOwner(WeaponSocketName);
				}
				// If the equipment being equipped or unequipped is a shield.
				if (EquipmentType == EEquipmentType::Shield) {
					// Equips or unequips the shield if the weapon is a SwordShield.
					if (MainWeapon->GetCombatType() == ECombatType::SwordShield) {
						if (AShield* Shield = CombatComponent->GetShield()) {
							FName ShieldSocketName;
							// For the equip animation, sets the socket name to EquipSocket.
							if (MontageActionTag == MyGameplayTags::Character_Action_Equip) {
								ShieldSocketName = Shield->GetEquipSocketName();
							}
							// For the unequip animation, sets the socket name to UnequipSocket.
							else if (MontageActionTag == MyGameplayTags::Character_Action_Unequip) {
								ShieldSocketName = Shield->GetUnequipSocketName();
							}
							Shield->AttachToOwner(ShieldSocketName);
						}
					}
				}
			}
		}
	}
}