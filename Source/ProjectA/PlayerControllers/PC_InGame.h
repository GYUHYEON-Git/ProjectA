// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PC_InGame.generated.h"

class UPlayerHUDWidget;
class UPauseMenuWidget;
class UAudioComponent;
class USoundBase;
class UInputMappingContext;
class UInputAction;
class UDemoEndWidget;
class UDiedWidget;
/**
 * 
 */
UCLASS()
class PROJECTA_API APC_InGame : public APlayerController
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UPlayerHUDWidget> PlayerHUDWidgetClass;

	UPROPERTY()
	TObjectPtr<UPlayerHUDWidget> PlayerHUDWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UPauseMenuWidget> PauseMenuWidgetClass;

	UPROPERTY()
	TObjectPtr<UPauseMenuWidget> PauseMenuWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UDemoEndWidget> DemoEndWidgetClass;

	UPROPERTY()
	TObjectPtr<UDemoEndWidget> DemoEndWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UDiedWidget> DiedWidgetClass;

	UPROPERTY()
	TObjectPtr<UDiedWidget> DiedWidget;

	UPROPERTY()
	TObjectPtr<UAudioComponent> BGMAudioComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	TObjectPtr<USoundBase> InGameBGM;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> InGameMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> PauseAction;

	bool bStartedMusic = false;

	bool bPausing = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Volume = 0.5f;

protected:
	virtual void BeginPlay() override;

public:
	virtual void SetupInputComponent() override;

	FORCEINLINE void SetPausing(bool bPause) { bPausing = bPause; }

	UFUNCTION(BlueprintCallable, Category = "UI")
	UPlayerHUDWidget* GetPlayerHUDWidget() const { return PlayerHUDWidget; }

	UFUNCTION(BlueprintCallable, Category = "Audio")
	void StartInGameBGM(float FadeInDuration = 1.0f);

	UFUNCTION(BlueprintCallable, Category = "Audio")
	void StopInGameBGM(float FadeOutDuration = 1.0f);

	UFUNCTION(BlueprintCallable, Category = "Audio")
	void TogglePauseMenu();

	void ShowDemoEndMenu();

	void ShowGameOver();
	
};
