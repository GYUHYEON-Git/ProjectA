// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/AnimNotifyState_WeaponCollision.h"

#include "Interfaces/CombatInterface.h"

//#include "Components/CombatComponent.h"
//#include "Components/WeaponCollisionComponent.h"
//#include "Equipments/Weapon.h"


UAnimNotifyState_WeaponCollision::UAnimNotifyState_WeaponCollision(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

void UAnimNotifyState_WeaponCollision::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) {
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	if (AActor* OwnerActor = MeshComp->GetOwner()) {
		if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(OwnerActor)) {
			CombatInterface->ActivateWeaponCollision(CollisionType);
		}
	}
}

void UAnimNotifyState_WeaponCollision::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) {
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	if (AActor* OwnerActor = MeshComp->GetOwner()) {
		if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(OwnerActor)) {
			CombatInterface->DeactivateWeaponCollision(CollisionType);
		}
	}
}