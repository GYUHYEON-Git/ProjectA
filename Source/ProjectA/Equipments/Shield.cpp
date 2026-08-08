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

			// 무기가 이미 있는지 체크해보고 방패의 소켓 위치를 조정해준다.
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
