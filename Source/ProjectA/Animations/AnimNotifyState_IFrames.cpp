// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/AnimNotifyState_IFrames.h"

#include "Interfaces/CombatInterface.h"

// Marks the character as unable to attack when the Notify State begins.
void UAnimNotifyState_IFrames::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) {
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	AActor* OwnerActor = MeshComp->GetOwner();
	if (!OwnerActor) return;
	if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(OwnerActor)) {
		CombatInterface->ToggleIFrames(true);
	}
}
// Removes the attack restriction when the Notify State ends.
void UAnimNotifyState_IFrames::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) {
	AActor* OwnerActor = MeshComp->GetOwner();
	if (!OwnerActor) return;
	if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(OwnerActor)) {
		CombatInterface->ToggleIFrames(false);
	}
	Super::NotifyEnd(MeshComp, Animation, EventReference);
}
