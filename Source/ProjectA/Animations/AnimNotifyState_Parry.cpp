// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/AnimNotifyState_Parry.h"
#include "AnimNotifyState_Parry.h"
#include "Components/StateComponent.h"

// Enters the Parry state when the Notify State begins.
void UAnimNotifyState_Parry::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) {
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	if (const AActor* OwnerActor = MeshComp->GetOwner()) {
		if (UStateComponent* StateComponent = OwnerActor->GetComponentByClass<UStateComponent>()) {
			StateComponent->SetState(MyGameplayTags::Character_State_Parrying);
		}
	}
}
// Resets the state when the Notify State ends. If an enemy attacks during the configured window, triggers a Parry.
void UAnimNotifyState_Parry::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) {
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	if (const AActor* OwnerActor = MeshComp->GetOwner()) {
		if (UStateComponent* StateComponent = OwnerActor->GetComponentByClass<UStateComponent>()) {
			StateComponent->ClearState();
		}
	}
}
