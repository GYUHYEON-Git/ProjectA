// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/AnimNotifyState_WeaponCollision.h"

#include "Interfaces/CombatInterface.h"

UAnimNotifyState_WeaponCollision::UAnimNotifyState_WeaponCollision(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}
// Starts weapon collision detection when the Notify State begins.
void UAnimNotifyState_WeaponCollision::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) {
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	if (AActor* OwnerActor = MeshComp->GetOwner()) {
		if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(OwnerActor)) {
			CombatInterface->ActivateWeaponCollision(CollisionType);
		}
	}
}
// Stops weapon collision detection when the Notify State ends. Applies damage if a collision target is detected.
void UAnimNotifyState_WeaponCollision::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) {
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	if (AActor* OwnerActor = MeshComp->GetOwner()) {
		if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(OwnerActor)) {
			CombatInterface->DeactivateWeaponCollision(CollisionType);
		}
	}
}