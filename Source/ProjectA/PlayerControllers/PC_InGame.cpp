// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerControllers/PC_InGame.h"

#include "UI/PlayerHUDWidget.h"

void APC_InGame::BeginPlay() {
	Super::BeginPlay();
	// 인게임 PlayerController의 BeginPlay() 등
	bShowMouseCursor = false;

	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);
	// Create the player HUD
	if (PlayerHUDWidgetClass) {
		PlayerHUDWidget = CreateWidget<UPlayerHUDWidget>(this, PlayerHUDWidgetClass);
		if (PlayerHUDWidget) {
			PlayerHUDWidget->AddToViewport();
		}
	}
}