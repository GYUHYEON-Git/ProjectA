// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModes/GM_InGame.h"

#include "Kismet/GameplayStatics.h"
#include "Characters/EnemyCharacter.h"
#include "PlayerControllers/PC_InGame.h"

void AGM_InGame::BeginPlay() {
	Super::BeginPlay();
	TArray<AActor*> FoundEnemies;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemyCharacter::StaticClass(), FoundEnemies);
	RemainingEnemies = FoundEnemies.Num();
}

void AGM_InGame::OnEnemyDied() {
	RemainingEnemies--;
	if (RemainingEnemies <= 0) {
		if (APC_InGame* PC = Cast<APC_InGame>(GetWorld()->GetFirstPlayerController())) {
			PC->ShowDemoEndMenu();
		}
	}
}

void AGM_InGame::GameOver() {
	if (APC_InGame* PC = Cast<APC_InGame>(GetWorld()->GetFirstPlayerController())) {
		PC->ShowGameOver();
	}
}
