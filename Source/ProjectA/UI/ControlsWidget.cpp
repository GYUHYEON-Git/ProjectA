// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ControlsWidget.h"

#include "UI/ButtonWidget.h"
#include "Components/Button.h"

UControlsWidget::UControlsWidget(const FObjectInitializer& ObjectInitializer) 
	:Super(ObjectInitializer){}

void UControlsWidget::NativeConstruct() {
	if (BackButton) {
		BackButton->SetText(FText::FromString(TEXT("X")));
		BackButton->GetButton()->OnClicked.AddDynamic(this, &ThisClass::HandleBackButtonClicked);
	}
}

void UControlsWidget::HandleBackButtonClicked() {
	if (OnBackButtonClicked.IsBound()) {
		OnBackButtonClicked.Broadcast();
	}
}