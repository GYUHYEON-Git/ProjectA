// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerControllers/PC_InGame.h"

#include "UI/PlayerHUDWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"

void APC_InGame::BeginPlay() {
	Super::BeginPlay();
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
	StartInGameBGM(1.f);
}

void APC_InGame::StartInGameBGM(float FadeInDuration) {
	if (InGameBGM) {
		if (!bStartedMusic) {
			bStartedMusic = true;
			BGMAudioComponent = UGameplayStatics::CreateSound2D(this, InGameBGM);
			if (BGMAudioComponent) {
				BGMAudioComponent->SetVolumeMultiplier(Volume);
				BGMAudioComponent->FadeIn(1.f);
			}
		}
	}
}

void APC_InGame::StopInGameBGM(float FadeOutDuration) {
	if (IsValid(BGMAudioComponent) && BGMAudioComponent->IsPlaying()) {
		bStartedMusic = false;
		BGMAudioComponent->FadeOut(FadeOutDuration, 0);
	}
}
