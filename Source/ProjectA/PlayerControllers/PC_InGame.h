// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PC_InGame.generated.h"

class UPlayerHUDWidget;
class UAudioComponent;
class USoundBase;
/**
 * 
 */
UCLASS()
class PROJECTA_API APC_InGame : public APlayerController
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UPlayerHUDWidget> PlayerHUDWidgetClass;

	UPROPERTY()
	TObjectPtr<UPlayerHUDWidget> PlayerHUDWidget;

	UPROPERTY()
	TObjectPtr<UAudioComponent> BGMAudioComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	TObjectPtr<USoundBase> InGameBGM;

	bool bStartedMusic = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Volume = 0.5f;

protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable, Category = "UI")
	UPlayerHUDWidget* GetPlayerHUDWidget() const { return PlayerHUDWidget; }

	UFUNCTION(BlueprintCallable, Category = "Audio")
	void StartInGameBGM(float FadeInDuration = 1.0f);

	UFUNCTION(BlueprintCallable, Category = "Audio")
	void StopInGameBGM(float FadeOutDuration = 1.0f);
	
};
