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

void UAnimNotify_EquipWeapon::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) {
	Super::Notify(MeshComp, Animation, EventReference);
	if (const AActor* Owner = MeshComp->GetOwner()) {
		if (UCombatComponent* CombatComponent = Owner->GetComponentByClass<UCombatComponent>()) {
			if (AWeapon* MainWeapon = CombatComponent->GetMainWeapon()) {
				// 장착/해제 대상이 무기인 경우
				if (EquipmentType == EEquipmentType::Weapon) {
					bool bCombatEnable = CombatComponent->IsCombatEnabled();
					FName WeaponSocketName;
					if (MontageActionTag == MyGameplayTags::Character_Action_Equip) {
						bCombatEnable = true;
						WeaponSocketName = MainWeapon->GetEquipSocketName();
					}
					else if (MontageActionTag == MyGameplayTags::Character_Action_Unequip) {
						bCombatEnable = false;
						WeaponSocketName = MainWeapon->GetUnequipSocketName();
					}
					CombatComponent->SetCombatEnabled(bCombatEnable);
					MainWeapon->AttachToOwner(WeaponSocketName);
				}
				// 장착/해제 대상이 방패인 경우
				if (EquipmentType == EEquipmentType::Shield) {
					// 무기가 한손검이면 방패를 장착/해제
					if (MainWeapon->GetCombatType() == ECombatType::SwordShield) {
						if (AShield* Shield = CombatComponent->GetShield()) {
							FName ShieldSocketName;
							if (MontageActionTag == MyGameplayTags::Character_Action_Equip) {
								ShieldSocketName = Shield->GetEquipSocketName();
							}
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