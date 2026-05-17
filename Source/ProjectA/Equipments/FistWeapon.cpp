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
			// 주먹을 무기로 사용하기 때문에 캐릭터의 스켈레탈메시로 설정해준다.
			// MainCollision
			WeaponCollision->SetWeaponMesh(OwnerCharacter->GetMesh());
			// SecondCollision
			SecondWeaponCollision->SetWeaponMesh(OwnerCharacter->GetMesh());

			// 주먹이 무기이기 때문에 여기서 세팅해준다.
			// 실제로 장착되는 무기들은 애니메이션의 Notify로 처리
			CombatComponent->SetCombatEnabled(true);

			// 장착한 무기의 CombatType으로 업데이트
			if (UMyAnimInstance* Anim = Cast<UMyAnimInstance>(OwnerCharacter->GetMesh()->GetAnimInstance())) {
				Anim->UpdateCombatMode(CombatType);
			}
			// 무기를 소유한 OwnerActor를 충돌에서 무시합니다
			WeaponCollision->AddIgnoredActor(OwnerCharacter);
			SecondWeaponCollision->AddIgnoredActor(OwnerCharacter);
		}
	}
}