// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MainMenuHUDWidget.h"
#include "UI/ButtonWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerControllers/PC_MainMenu.h"
#include "UI/ControlsWidget.h"
#include "Components/WidgetSwitcher.h"


UMainMenuHUDWidget::UMainMenuHUDWidget(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer) {
}

void UMainMenuHUDWidget::NativeConstruct() {
	Super::NativeConstruct();
	if (GameStartButton) {
		GameStartButton->SetText(FText::FromString(TEXT("GameStart")), 36.f);
		GameStartButton->GetButton()->OnClicked.AddDynamic(this, &ThisClass::OnGameStartButtonClicked);
	}
	if (ControlsButton) {
		ControlsButton->SetText(FText::FromString(TEXT("Controls")), 36.f);
		ControlsButton->GetButton()->OnClicked.AddDynamic(this, &ThisClass::OnControlsButtonClicked);
	}
	if (QuitButton) {
		QuitButton->SetText(FText::FromString(TEXT("Quit")), 36.f);
		QuitButton->GetButton()->OnClicked.AddDynamic(this, &ThisClass::OnQuitButtonClicked);
	}
	if (ControlsWidget) {
		ControlsWidget->OnBackButtonClicked.AddDynamic(this, &ThisClass::OnControlsBackButtonClicked);
	}
}

void UMainMenuHUDWidget::OnGameStartButtonClicked() {
	if (APC_MainMenu* PC = Cast<APC_MainMenu>(GetOwningPlayer())) {
		PC->StopMainMenuBGM(1.f);
	}
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([this]() {
		UGameplayStatics::OpenLevel(this, FName("LV_Tutorial"));
		}), 1.0f, false);
}

void UMainMenuHUDWidget::OnControlsButtonClicked() {
	if (WidgetSwitcher) {
		WidgetSwitcher->SetActiveWidgetIndex(1);
	}
}

void UMainMenuHUDWidget::OnQuitButtonClicked() {
	APlayerController* PC = GetOwningPlayer();
	UKismetSystemLibrary::QuitGame(
		this,
		PC,
		EQuitPreference::Quit,
		false
	);
}

void UMainMenuHUDWidget::OnControlsBackButtonClicked() {
	if (WidgetSwitcher) {
		WidgetSwitcher->SetActiveWidgetIndex(0);
	}
}
