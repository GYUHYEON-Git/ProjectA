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

	// ½ºÅÏ
	if (StateComponent->IsCurrentStateEqualToAny(CheckTags)) {
		SetBehaviorKey(BlackboardComp, EAIBehavior::Stunned);
	}
	else {
		AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(TargetKey.SelectedKeyName));
		// Check if target exists
		if (IsValid(TargetActor)) {
			const float Distance = TargetActor->GetDistanceTo(ControlledPawn);
			// Check if within attack range
			if (Distance <= AttackRangeDistance) {
				SetBehaviorKey(BlackboardComp, EAIBehavior::MeleeAttack);
			}
			else {
				SetBehaviorKey(BlackboardComp, EAIBehavior::Approach);
			}
		}
		else {
			// Check if patrol point is available
			if (ControlledPawn->GetPatrolPoint() != nullptr) {
				SetBehaviorKey(BlackboardComp, EAIBehavior::Patrol);
			}
			else {
				SetBehaviorKey(BlackboardComp, EAIBehavior::Idle);
			}
		}
	}
}