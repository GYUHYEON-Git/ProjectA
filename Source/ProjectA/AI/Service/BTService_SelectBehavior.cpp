// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Service/BTService_SelectBehavior.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/EnemyCharacter.h"

UBTService_SelectBehavior::UBTService_SelectBehavior() {
	INIT_SERVICE_NODE_NOTIFY_FLAGS();
}

void UBTService_SelectBehavior::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) {
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);

	APawn* ControlledPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (!ControlledPawn) return;
	ControlledEnemy = Cast<AEnemyCharacter>(ControlledPawn);
}

void UBTService_SelectBehavior::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) {
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	UpdateBehavior(OwnerComp.GetBlackboardComponent());
}

void UBTService_SelectBehavior::SetBehaviorKey(UBlackboardComponent* BlackboardComp, EAIBehavior Behavior) const {
	BlackboardComp->SetValueAsEnum(BehaviorKey.SelectedKeyName, static_cast<uint8>(Behavior));
}

void UBTService_SelectBehavior::UpdateBehavior(UBlackboardComponent* BlackboardComp) const {
	check(BlackboardComp)
	check(ControlledEnemy)

	AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(TargetKey.SelectedKeyName));
	if (IsValid(TargetActor)) {
		const float Distance = TargetActor->GetDistanceTo(ControlledEnemy.Get());

		// 공격범위 안쪽이면
		if (Distance <= AttackRangeDistance) {
			SetBehaviorKey(BlackboardComp, EAIBehavior::MeleeAttack);
		}
		else {
			SetBehaviorKey(BlackboardComp, EAIBehavior::Approach);
		}
	}
	else {
		// Patrol point 있으면
		if (ControlledEnemy->GetPatrolPoint() != nullptr) {
			SetBehaviorKey(BlackboardComp, EAIBehavior::Patrol);
		}
		else {
			SetBehaviorKey(BlackboardComp, EAIBehavior::Idle);
		}
	}
}