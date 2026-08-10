// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PC_InGame.generated.h"

class UPlayerHUDWidget;
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

protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable, Category = "UI")
	UPlayerHUDWidget* GetPlayerHUDWidget() const { return PlayerHUDWidget; }
	
};
