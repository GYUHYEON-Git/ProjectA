// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/EnemyAIController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AIPerceptionComponent.h"
#include "Characters/EnemyCharacter.h"
#include "Components/RotationComponent.h"
#include "Components/MusicComponent.h"
#include "Components/StateComponent.h"

AEnemyAIController::AEnemyAIController() {
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>("AIPerception");
}

void AEnemyAIController::OnPossess(APawn* InPawn) {
	Super::OnPossess(InPawn);
	ControlledEnemy = Cast<AEnemyCharacter>(InPawn);
	RunBehaviorTree(BehaviorTreeAsset);
	// Register UpdateTarget timer
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ThisClass::UpdateTarget, 0.1f, true);
}

void AEnemyAIController::OnUnPossess() {
	ControlledEnemy = nullptr;
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
	Super::OnUnPossess();
}

void AEnemyAIController::UpdateTarget() const {
	TArray<AActor*> OutActors;
	AIPerceptionComponent->GetKnownPerceivedActors(nullptr, OutActors);

	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	UMusicComponent* MusicComponent = ControlledEnemy->GetComponentByClass<UMusicComponent>();
	UStateComponent* StateComponent = ControlledEnemy->GetComponentByClass<UStateComponent>();

	if (!MusicComponent) return;

	if (StateComponent->GetCurrentState() == MyGameplayTags::Character_State_Death) {
		SetTarget(nullptr);
		ControlledEnemy->ToggleHealthBarVisibility(false);
		MusicComponent->StopMusic();
	}

	if (OutActors.Contains(PlayerCharacter)) {
		if (!PlayerCharacter->IsDeath()) {
			SetTarget(PlayerCharacter);
			ControlledEnemy->ToggleHealthBarVisibility(true);
			MusicComponent->StartMusic();
		}
		else {
			SetTarget(nullptr);
			ControlledEnemy->ToggleHealthBarVisibility(false);
			MusicComponent->StopMusic();
		}
	}
	else {
		SetTarget(nullptr);
		ControlledEnemy->ToggleHealthBarVisibility(false);
		MusicComponent->StopMusic();
	}
}

void AEnemyAIController::SetTarget(AActor* NewTarget) const {
	if (IsValid(Blackboard)) {
		Blackboard->SetValueAsObject(FName("Target"), NewTarget);
	}
	if (URotationComponent* RotationComponent = ControlledEnemy->GetComponentByClass<URotationComponent>()) {
		RotationComponent->SetTargetActor(NewTarget);
	}
}
