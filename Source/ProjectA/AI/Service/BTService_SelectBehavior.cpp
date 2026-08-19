// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Service/BTService_SelectBehavior.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/EnemyCharacter.h"
#include "MyGameplayTags.h"
#include "Components/StateComponent.h"

UBTService_SelectBehavior::UBTService_SelectBehavior() {
	INIT_SERVICE_NODE_NOTIFY_FLAGS();
}

void UBTService_SelectBehavior::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) {
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	UpdateBehavior(OwnerComp, OwnerComp.GetBlackboardComponent());
}

void UBTService_SelectBehavior::SetBehaviorKey(UBlackboardComponent* BlackboardComp, EAIBehavior Behavior) const {
	BlackboardComp->SetValueAsEnum(BehaviorKey.SelectedKeyName, static_cast<uint8>(Behavior));
}

void UBTService_SelectBehavior::UpdateBehavior(UBehaviorTreeComponent& OwnerComp, UBlackboardComponent* BlackboardComp) const {
	check(BlackboardComp);

	AEnemyCharacter* ControlledPawn = Cast<AEnemyCharacter>(OwnerComp.GetAIOwner()->GetPawn());
	if (!ControlledPawn) return;

	const UStateComponent* StateComponent = ControlledPawn->GetComponentByClass<UStateComponent>();
	check(StateComponent);

	FGameplayTagContainer CheckTags;
	CheckTags.AddTag(MyGameplayTags::Character_State_Parried);
	CheckTags.AddTag(MyGameplayTags::Character_State_Stunned);

	// If the enemy is stunned or parried, set the behavior to Stunned.
	if (StateComponent->IsCurrentStateEqualToAny(CheckTags)) {
		SetBehaviorKey(BlackboardComp, EAIBehavior::Stunned);
	}
	else {
		AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(TargetKey.SelectedKeyName));
		// If a target exists.
		if (IsValid(TargetActor)) {
			const float Distance = TargetActor->GetDistanceTo(ControlledPawn);
			// If the target is within attack range, set the behavior to Attack.
			if (Distance <= AttackRangeDistance) {
				SetBehaviorKey(BlackboardComp, EAIBehavior::MeleeAttack);
			}
			// If the target is outside attack range, set the behavior to Approach.
			else {
				SetBehaviorKey(BlackboardComp, EAIBehavior::Approach);
			}
		}
		// If no target exists.
		else {
			// If a target point exists, set the behavior to Patrol.
			if (ControlledPawn->GetPatrolPoint() != nullptr) {
				SetBehaviorKey(BlackboardComp, EAIBehavior::Patrol);
			}
			// If no target point exists, set the behavior to Idle.
			else {
				SetBehaviorKey(BlackboardComp, EAIBehavior::Idle);
			}
		}
	}
}