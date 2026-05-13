// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/AnimNotify_AttackFinished.h"

#include "Characters/PlayerCharacter.h"

UAnimNotify_AttackFinished::UAnimNotify_AttackFinished(const FObjectInitializer& ObjectInitializer) 
	:Super(ObjectInitializer) {}

void UAnimNotify_AttackFinished::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) {
	Super::Notify(MeshComp, Animation, EventReference);
	if (APlayerCharacter* Character = Cast<APlayerCharacter>(MeshComp->GetOwner())) {
		Character->AttackFinished(ComboResetDelay);
	}
}