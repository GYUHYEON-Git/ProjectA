// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerControllers/PC_MainMenu.h"

#include "UI/MainMenuHUDWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"

void APC_MainMenu::BeginPlay() {
	Super::BeginPlay();

	// 1. 마우스 커서 보이게 설정
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;

	// 2. 입력 모드를 UI 전용(또는 UI and Game)으로 설정하여 마우스가 화면 밖으로 나가지 않게 고정
	FInputModeUIOnly InputMode;
	// FInputModeGameAndUI InputMode; // 게임 입력도 같이 받으려면 사용

	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	SetInputMode(InputMode);



	// Create the player HUD
	if (MainMenuHUDWidgetClass) {
		MainMenuHUDWidget = CreateWidget<UMainMenuHUDWidget>(this, MainMenuHUDWidgetClass);
		if (MainMenuHUDWidget) {
			MainMenuHUDWidget->AddToViewport();
		}
	}
	if (MainMenuBGM) {
		BGMAudioComponent = UGameplayStatics::SpawnSound2D(this, MainMenuBGM);
		if (BGMAudioComponent) {
			BGMAudioComponent->FadeIn(1.0f);
		}
	}
}

void APC_MainMenu::StopMainMenuBGM(float FadeOutDuration) {
	if (BGMAudioComponent && BGMAudioComponent->IsPlaying()) {
		BGMAudioComponent->FadeOut(FadeOutDuration, 0.0f);
	}
}
