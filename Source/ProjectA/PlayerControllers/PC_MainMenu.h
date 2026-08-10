// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PC_MainMenu.generated.h"

class UMainMenuHUDWidget;
class UAudioComponent;
class USoundBase;
/**
 * 
 */
UCLASS()
class PROJECTA_API APC_MainMenu : public APlayerController
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UMainMenuHUDWidget> MainMenuHUDWidgetClass;

	UPROPERTY()
	TObjectPtr<UMainMenuHUDWidget> MainMenuHUDWidget;

	UPROPERTY()
	TObjectPtr<UAudioComponent> BGMAudioComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	TObjectPtr<USoundBase> MainMenuBGM;

protected:
	virtual void BeginPlay() override;


public:
	UFUNCTION(BlueprintCallable, Category = "Audio")
	void StopMainMenuBGM(float FadeOutDuration = 1.0f);

};
