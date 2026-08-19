// Fill out your copyright notice in the Description page of Project Settings.


#include "Equipments/FistWeapon.h"

#include "Characters/PlayerCharacter.h"
#include "Animations/MyAnimInstance.h"
#include "Components/CombatComponent.h"
#include "Components/WeaponCollisionComponent.h"

AFistWeapon::AFistWeapon(){}

void AFistWeapon::EquipItem() {
	CombatComponent = GetOwner()->GetComponentByClass<UCombatComponent>();
	if (CombatComponent) {
		CombatComponent->SetWeapon(this);
		if (APlayerCharacter* OwnerCharacter = Cast<APlayerCharacter>(GetOwner())) {
			WeaponCollision->SetWeaponMesh(OwnerCharacter->GetMesh());
			SecondWeaponCollision->SetWeaponMesh(OwnerCharacter->GetMesh());

			CombatComponent->SetCombatEnabled(true);

			if (UMyAnimInstance* Anim = Cast<UMyAnimInstance>(OwnerCharacter->GetMesh()->GetAnimInstance())) {
				Anim->UpdateCombatMode(CombatType);
			}
			WeaponCollision->AddIgnoredActor(OwnerCharacter);
			SecondWeaponCollision->AddIgnoredActor(OwnerCharacter);
		}
	}
}