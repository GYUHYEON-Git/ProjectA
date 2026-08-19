// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/AnimNotifyState_RotateTo.h"

#include "Components/RotationComponent.h"

// Rotates toward the target when the Notify State begins.
void UAnimNotifyState_RotateTo::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) {
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	if (URotationComponent* RotationComponent = MeshComp->GetOwner()->GetComponentByClass<URotationComponent>()) {
		RotationComponent->ToggleShouldRotate(true);
	}
}
// Stops rotating when the Notify State ends. Placed near the beginning of attack animations so the character can attack toward the target.
void UAnimNotifyState_RotateTo::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) {
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	if (URotationComponent* RotationComponent = MeshComp->GetOwner()->GetComponentByClass<URotationComponent>()) {
		RotationComponent->ToggleShouldRotate(false);
	}
}