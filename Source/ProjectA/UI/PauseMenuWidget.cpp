// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PauseMenuWidget.h"

#include "Kismet/GameplayStatics.h"
#include "Components/WidgetSwitcher.h"
#include "UI/ButtonWidget.h"
#include "Components/Button.h"
#include "UI/ControlsWidget.h"
#include "DiedWidget.h"

UPauseMenuWidget::UPauseMenuWidget(const FObjectInitializer& ObjectInitializer) 
    :Super(ObjectInitializer) {}

void UPauseMenuWidget::NativeConstruct() {
	Super::NativeConstruct();
	if (ResumeButton) {
		ResumeButton->SetText(FText::FromString(TEXT("Resume")));
		ResumeButton->GetButton()->OnClicked.AddDynamic(this, &ThisClass::OnResumeButtonClicked);
	}
	if (ControlsButton) {
		ControlsButton->SetText(FText::FromString(TEXT("Controls")));
		ControlsButton->GetButton()->OnClicked.AddDynamic(this, &ThisClass::OnControlsButtonClicked);
	}
	if (MainMenuButton) {
		MainMenuButton->SetText(FText::FromString(TEXT("MainMenu")));
		MainMenuButton->GetButton()->OnClicked.AddDynamic(this, &ThisClass::OnMainMenuButtonClicked);
	}
	if (ControlsWidget) {
		ControlsWidget->OnBackButtonClicked.AddDynamic(this, &ThisClass::OnControlsBackButtonClicked);
	}
}

void UPauseMenuWidget::OnResumeButtonClicked() {
    UGameplayStatics::SetGamePaused(GetWorld(), false);
    if (APlayerController* PC = GetOwningPlayer()) {
        FInputModeGameOnly InputMode;
        PC->SetInputMode(InputMode);
        PC->bShowMouseCursor = false;
    }
    RemoveFromParent();
}

void UPauseMenuWidget::OnControlsButtonClicked() {
    if (WidgetSwitcher) {
        WidgetSwitcher->SetActiveWidgetIndex(1);
    }
}

void UPauseMenuWidget::OnMainMenuButtonClicked() {
	UGameplayStatics::SetGamePaused(GetWorld(), false);
	UGameplayStatics::OpenLevel(this, FName("Lv_MainMenu"));
}

void UPauseMenuWidget::OnControlsBackButtonClicked() {
	if (WidgetSwitcher) {
		WidgetSwitcher->SetActiveWidgetIndex(0);
	}
}
