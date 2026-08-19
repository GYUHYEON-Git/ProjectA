// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerControllers/PC_MainMenu.h"

#include "UI/MainMenuHUDWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"

void APC_MainMenu::BeginPlay() {
	Super::BeginPlay();
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;

	FInputModeUIOnly InputMode;

	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	SetInputMode(InputMode);

	if (MainMenuHUDWidgetClass) {
		MainMenuHUDWidget = CreateWidget<UMainMenuHUDWidget>(this, MainMenuHUDWidgetClass);
		if (MainMenuHUDWidget) {
			MainMenuHUDWidget->AddToViewport();
		}
	}
	StartMainMenuBGM(3.f);
}

void APC_MainMenu::StartMainMenuBGM(float FadeInDuration) {
	if (MainMenuBGM) {
		BGMAudioComponent = UGameplayStatics::CreateSound2D(this, MainMenuBGM);
		if (BGMAudioComponent) {
			BGMAudioComponent->FadeIn(FadeInDuration);
		}
	}
}

void APC_MainMenu::StopMainMenuBGM(float FadeOutDuration) {
	if (BGMAudioComponent && BGMAudioComponent->IsPlaying()) {
		BGMAudioComponent->FadeOut(FadeOutDuration, 0.0f);
	}
}
