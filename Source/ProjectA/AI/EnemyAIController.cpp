// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/EnemyAIController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AIPerceptionComponent.h"
#include "Characters/EnemyCharacter.h"
#include "Components/RotationComponent.h"

AEnemyAIController::AEnemyAIController() {
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>("AIPerception");
}

void AEnemyAIController::OnPossess(APawn* InPawn) {
	Super::OnPossess(InPawn);
	ControlledEnemy = Cast<AEnemyCharacter>(InPawn);
	RunBehaviorTree(BehaviorTreeAsset);
	// UpdateTarget 타이머 등록
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

	if (OutActors.Contains(PlayerCharacter)) {
		if (!PlayerCharacter->IsDeath()) {
			SetTarget(PlayerCharacter);
			ControlledEnemy->ToggleHealthBarVisibility(true);
		}
		else {
			SetTarget(nullptr);
			ControlledEnemy->ToggleHealthBarVisibility(false);
		}
	}
	else {
		SetTarget(nullptr);
		ControlledEnemy->ToggleHealthBarVisibility(false);
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
