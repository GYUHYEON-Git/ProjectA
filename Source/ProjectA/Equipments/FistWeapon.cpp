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
			// Use the character's skeletal mesh because the fists act as the weapon
			// MainCollision
			WeaponCollision->SetWeaponMesh(OwnerCharacter->GetMesh());
			// SecondCollision
			SecondWeaponCollision->SetWeaponMesh(OwnerCharacter->GetMesh());

			// Configure this here because the fists are used as the weapon
			// Regular weapons are equipped through animation notifies
			CombatComponent->SetCombatEnabled(true);

			// Update the CombatType based on the equipped weapon
			if (UMyAnimInstance* Anim = Cast<UMyAnimInstance>(OwnerCharacter->GetMesh()->GetAnimInstance())) {
				Anim->UpdateCombatMode(CombatType);
			}
			// Ignore the owning actor during collision checks
			WeaponCollision->AddIgnoredActor(OwnerCharacter);
			SecondWeaponCollision->AddIgnoredActor(OwnerCharacter);
		}
	}
}