// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Task/BTTask_PerformAttack.h"

#include "AIController.h"
#include "Components/StateComponent.h"
#include "Interfaces/CombatInterface.h"
#include "BehaviorTree/BlackboardComponent.h"

// TaskNode responsible for attacks in the Behavior Tree.
EBTNodeResult::Type UBTTask_PerformAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) {
	APawn* ControlledPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (!ControlledPawn) { return EBTNodeResult::Failed; }

	UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
	if (!BBComp) return EBTNodeResult::Failed;

	if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(ControlledPawn)) {
		FOnMontageEnded MontageEndedDelegate;
		MontageEndedDelegate.BindLambda([this, &OwnerComp, ControlledPawn](UAnimMontage* Montage, bool bInterrupted) {
			// Executed after the attack animation ends.
			if (!::IsValid(&OwnerComp)) return;
			if (UStateComponent* StateComponent = ControlledPawn->GetComponentByClass<UStateComponent>()) {
				// Resets the state unless the enemy is parried or stunned.
				FGameplayTagContainer CheckTags;
				CheckTags.AddTag(MyGameplayTags::Character_State_Parried);
				CheckTags.AddTag(MyGameplayTags::Character_State_Stunned);
				if (!StateComponent->IsCurrentStateEqualToAny(CheckTags)) {
					StateComponent->ClearState();
				}
			}
			// Sets the attack state flag to false.
			if (UBlackboardComponent* LambdaBBComp = OwnerComp.GetBlackboardComponent()) {
				LambdaBBComp->SetValueAsBool(BlackboardbIsAttacking.SelectedKeyName, false);
			}
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		});
		// Sets the attack state flag to true.
		BBComp->SetValueAsBool(BlackboardbIsAttacking.SelectedKeyName, true);
		// Execute attack
		CombatInterface->PerformAttack(AttackTypeTag, MontageEndedDelegate);
		return EBTNodeResult::InProgress;
	}
	return EBTNodeResult::Failed;
}

EBTNodeResult::Type UBTTask_PerformAttack::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) {
	// Executed when the attack animation does not end normally due to states such as Stunned or Parried.
	if (UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent()) {
		// Forces the attack state flag to false.
		BBComp->SetValueAsBool(BlackboardbIsAttacking.SelectedKeyName, false);
	}
	return Super::AbortTask(OwnerComp, NodeMemory);
}