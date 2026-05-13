// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/AnimNotifyState_ComboWindow.h"

#include "Characters/PlayerCharacter.h"

UAnimNotifyState_ComboWindow::UAnimNotifyState_ComboWindow(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer) {}

void UAnimNotifyState_ComboWindow::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) {
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	if (APlayerCharacter* Character = Cast<APlayerCharacter>(MeshComp->GetOwner())) {
		Character->EnableComboWindow();
	}
}

void UAnimNotifyState_ComboWindow::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) {
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	if (APlayerCharacter* Character = Cast<APlayerCharacter>(MeshComp->GetOwner())) {
		Character->DisableComboWindow();
	}
}