// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Task/BTTask_FindPatrolPoint.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/EnemyCharacter.h"
#include "Engine/TargetPoint.h"

EBTNodeResult::Type UBTTask_FindPatrolPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) {
	APawn* ControlledPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (!ControlledPawn) {
		return EBTNodeResult::Failed;
	}
	// Accesses the target points stored in the enemy character in index order.
	if (AEnemyCharacter* Character = Cast<AEnemyCharacter>(ControlledPawn)) {
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(BlackboardLocation.SelectedKeyName, Character->GetPatrolPoint()->GetActorLocation());
		Character->IncrementPatrolIndex();
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}