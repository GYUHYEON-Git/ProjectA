// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GM_InGame.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTA_API AGM_InGame : public AGameModeBase
{
	GENERATED_BODY()

private:
	int32 RemainingEnemies = 0;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<class UUserWidget> DemoEndWidgetClass;

public:
	virtual void BeginPlay() override;

	void OnEnemyDied();

	void GameOver();

};
