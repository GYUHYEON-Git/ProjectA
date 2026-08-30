// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PauseMenuWidget.h"

#include "Kismet/GameplayStatics.h"
#include "Components/WidgetSwitcher.h"
#include "UI/ButtonWidget.h"
#include "Components/Button.h"
#include "UI/ControlsWidget.h"
#include "DiedWidget.h"
#include "PlayerControllers/PC_InGame.h"

UPauseMenuWidget::UPauseMenuWidget(const FObjectInitializer& ObjectInitializer) 
    :Super(ObjectInitializer) {}

void UPauseMenuWidget::NativeConstruct() {
	Super::NativeConstruct();
	if (CharacterButton) {
		CharacterButton->SetText(FText::FromString(TEXT("Character")), 24.f);
		CharacterButton->GetButton()->OnClicked.AddDynamic(this, &ThisClass::OnCharacterButtonClicked);
	}
	if (StatusButton) {
		StatusButton->SetText(FText::FromString(TEXT("Status")), 24.f);
		StatusButton->GetButton()->OnClicked.AddDynamic(this, &ThisClass::OnStatusButtonClicked);
	}
	if (OptionButton) {
		OptionButton->SetText(FText::FromString(TEXT("Option")), 24.f);
		OptionButton->GetButton()->OnClicked.AddDynamic(this, &ThisClass::OnOptionButtonClicked);
	}
	if (ResumeButton) {
		ResumeButton->SetText(FText::FromString(TEXT("Resume")), 36.f);
		ResumeButton->GetButton()->OnClicked.AddDynamic(this, &ThisClass::OnResumeButtonClicked);
	}
	if (ControlsButton) {
		ControlsButton->SetText(FText::FromString(TEXT("Controls")), 36.f);
		ControlsButton->GetButton()->OnClicked.AddDynamic(this, &ThisClass::OnControlsButtonClicked);
	}
	if (MainMenuButton) {
		MainMenuButton->SetText(FText::FromString(TEXT("MainMenu")), 36.f);
		MainMenuButton->GetButton()->OnClicked.AddDynamic(this, &ThisClass::OnMainMenuButtonClicked);
	}
	if (ControlsWidget) {
		ControlsWidget->OnBackButtonClicked.AddDynamic(this, &ThisClass::OnControlsBackButtonClicked);
	}
}

void UPauseMenuWidget::OnCharacterButtonClicked() {
	if (WidgetSwitcher) {
		WidgetSwitcher->SetActiveWidgetIndex(0);
		if (CharacterButton) {
			CharacterButton->SetColorAndOpacity(FLinearColor(0.95f, 0.78f, 0.42f, 1.f));
		}
		if (StatusButton) {
			StatusButton->SetColorAndOpacity(FLinearColor(0.52f, 0.46f, 0.4f, 1.f));
		}
		if (OptionButton) {
			OptionButton->SetColorAndOpacity(FLinearColor(0.52f, 0.46f, 0.4f, 1.f));
		}
	}
}

void UPauseMenuWidget::OnStatusButtonClicked() {
	if (WidgetSwitcher) {
		WidgetSwitcher->SetActiveWidgetIndex(1);
		if (CharacterButton) {
			CharacterButton->SetColorAndOpacity(FLinearColor(0.52f, 0.46f, 0.4f, 1.f));
		}
		if (StatusButton) {
			StatusButton->SetColorAndOpacity(FLinearColor(0.95f, 0.78f, 0.42f, 1.f));
		}
		if (OptionButton) {
			OptionButton->SetColorAndOpacity(FLinearColor(0.52f, 0.46f, 0.4f, 1.f));
		}
	}
}

void UPauseMenuWidget::OnOptionButtonClicked() {
	if (WidgetSwitcher) {
		WidgetSwitcher->SetActiveWidgetIndex(2);
		if (CharacterButton) {
			CharacterButton->SetColorAndOpacity(FLinearColor(0.52f, 0.46f, 0.4f, 1.f));
		}
		if (StatusButton) {
			StatusButton->SetColorAndOpacity(FLinearColor(0.52f, 0.46f, 0.4f, 1.f));
		}
		if (OptionButton) {
			OptionButton->SetColorAndOpacity(FLinearColor(0.95f, 0.78f, 0.42f, 1.f));
		}
	}
}

void UPauseMenuWidget::OnResumeButtonClicked() {
    UGameplayStatics::SetGamePaused(GetWorld(), false);
    if (APC_InGame* PC = Cast<APC_InGame>(GetOwningPlayer())) {
        FInputModeGameOnly InputMode;
        PC->SetInputMode(InputMode);
        PC->bShowMouseCursor = false;
		PC->SetPausing(false);
    }
    RemoveFromParent();
}

void UPauseMenuWidget::OnControlsButtonClicked() {
    if (WidgetSwitcherOption) {
		WidgetSwitcherOption->SetActiveWidgetIndex(1);
    }
}

void UPauseMenuWidget::OnMainMenuButtonClicked() {
	UGameplayStatics::SetGamePaused(GetWorld(), false);
	UGameplayStatics::OpenLevel(this, FName("Lv_MainMenu"));
}

void UPauseMenuWidget::OnControlsBackButtonClicked() {
	if (WidgetSwitcherOption) {
		WidgetSwitcherOption->SetActiveWidgetIndex(0);
	}
}
