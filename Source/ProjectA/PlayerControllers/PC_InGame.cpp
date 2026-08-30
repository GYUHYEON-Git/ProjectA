// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerControllers/PC_InGame.h"

#include "UI/PlayerHUDWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"
#include "UI/PauseMenuWidget.h"
#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "UI/DemoEndWidget.h"
#include "UI/DiedWidget.h"

void APC_InGame::BeginPlay() {
	Super::BeginPlay();
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer())) {
		if (InGameMappingContext) {
			Subsystem->AddMappingContext(InGameMappingContext, 1);
		}
	}
	bShowMouseCursor = false;
	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);

	if (PlayerHUDWidgetClass) {
		PlayerHUDWidget = CreateWidget<UPlayerHUDWidget>(this, PlayerHUDWidgetClass);
		if (PlayerHUDWidget) {
			PlayerHUDWidget->AddToViewport();
		}
	}
	StartInGameBGM(1.f);
}

void APC_InGame::SetupInputComponent() {
	Super::SetupInputComponent();
	if (UEnhancedInputComponent* EnhancedInputComp = Cast<UEnhancedInputComponent>(InputComponent)) {
		EnhancedInputComp->BindAction(PauseAction, ETriggerEvent::Started, this, &APC_InGame::TogglePauseMenu);
		PauseAction->bTriggerWhenPaused = true;
	}
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

void APC_InGame::TogglePauseMenu() {
	if (PauseMenuWidgetClass) {
		if (bPausing) {
			if (PauseMenuWidget) {
				UGameplayStatics::SetGamePaused(GetWorld(), false);
				FInputModeGameOnly InputMode;
				SetInputMode(InputMode);
				bShowMouseCursor = false;
				PauseMenuWidget->RemoveFromParent();
				bPausing = false;
			}
		}
		else {
			UGameplayStatics::SetGamePaused(GetWorld(), true);
			PauseMenuWidget = CreateWidget<UPauseMenuWidget>(this, PauseMenuWidgetClass);
			if (PauseMenuWidget) {
				PauseMenuWidget->AddToViewport();
				FInputModeGameAndUI InputMode;
				InputMode.SetWidgetToFocus(PauseMenuWidget->TakeWidget());
				InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
				SetInputMode(InputMode);
				int32 SizeX;
				int32 SizeY;
				GetViewportSize(SizeX, SizeY);
				SetMouseLocation(SizeX / 2, SizeY / 2);
				bShowMouseCursor = true;
				bPausing = true;
			}
		}
	}
}

void APC_InGame::ShowDemoEndMenu() {
	if (DemoEndWidgetClass) {
		DemoEndWidget = CreateWidget<UDemoEndWidget>(this, DemoEndWidgetClass);
		if (DemoEndWidget) {
			DemoEndWidget->AddToViewport();
			FInputModeGameAndUI InputMode;
			InputMode.SetWidgetToFocus(DemoEndWidget->TakeWidget());
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			SetInputMode(InputMode);
			bShowMouseCursor = true;
		}
	}
}

void APC_InGame::ShowGameOver() {
	if (DiedWidgetClass) {
		DiedWidget = CreateWidget<UDiedWidget>(this, DiedWidgetClass);
		if (DiedWidget) {
			DiedWidget->AddToViewport();
			FInputModeUIOnly InputMode;
			InputMode.SetWidgetToFocus(DiedWidget->TakeWidget());
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			SetInputMode(InputMode);
			bShowMouseCursor = true;
		}
	}
}