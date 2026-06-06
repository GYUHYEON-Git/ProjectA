// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/AnimNotifyState_RotateTo.h"

#include "Components/RotationComponent.h"

void UAnimNotifyState_RotateTo::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) {
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	if (URotationComponent* RotationComponent = MeshComp->GetOwner()->GetComponentByClass<URotationComponent>()) {
		RotationComponent->ToggleShouldRotate(true);
	}
}

void UAnimNotifyState_RotateTo::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) {
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	if (URotationComponent* RotationComponent = MeshComp->GetOwner()->GetComponentByClass<URotationComponent>()) {
		RotationComponent->ToggleShouldRotate(false);
	}
}