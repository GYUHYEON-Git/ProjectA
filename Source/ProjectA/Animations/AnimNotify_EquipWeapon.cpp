// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/AnimNotify_EquipWeapon.h"

#include "MyGameplayTags.h"
#include "Components/CombatComponent.h"
#include "Equipments/Weapon.h"

UAnimNotify_EquipWeapon::UAnimNotify_EquipWeapon(const FObjectInitializer& ObjectInitializer) 
	:Super(ObjectInitializer) {}

void UAnimNotify_EquipWeapon::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) {
	Super::Notify(MeshComp, Animation, EventReference);
	if (const AActor* Owner = MeshComp->GetOwner()) {
		if (UCombatComponent* CombatComponent = Owner->GetComponentByClass<UCombatComponent>()) {
			if (AWeapon* MainWeapon = CombatComponent->GetMainWeapon()) {
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
		}
	}
}