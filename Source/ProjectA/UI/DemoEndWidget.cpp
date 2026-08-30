// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/DemoEndWidget.h"

#include "UI/ButtonWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

UDemoEndWidget::UDemoEndWidget(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer) {}


void UDemoEndWidget::NativeConstruct() {
	if (CloseButton) {
		CloseButton->SetText(FText::FromString(TEXT("X")), 36.f);
		CloseButton->GetButton()->OnClicked.AddDynamic(this, &ThisClass::OnCloseButtonClicked);
	}
}

void UDemoEndWidget::OnCloseButtonClicked() {
	if (APlayerController* PC = GetOwningPlayer()) {
		FInputModeGameOnly InputMode;
		PC->SetInputMode(InputMode);
		PC->bShowMouseCursor = false;
	}
	RemoveFromParent();
}