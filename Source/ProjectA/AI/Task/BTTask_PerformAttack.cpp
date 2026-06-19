// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Task/BTTask_PerformAttack.h"

#include "AIController.h"
#include "Components/StateComponent.h"
#include "Interfaces/CombatInterface.h"
#include "BehaviorTree/BlackboardComponent.h"


EBTNodeResult::Type UBTTask_PerformAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) {
	APawn* ControlledPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (!ControlledPawn) { return EBTNodeResult::Failed; }

	UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
	if (!BBComp) return EBTNodeResult::Failed;

	if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(ControlledPawn)) {
		FOnMontageEnded MontageEndedDelegate;
		// Bind delegate
		MontageEndedDelegate.BindLambda([this, &OwnerComp, ControlledPawn](UAnimMontage* Montage, bool bInterrupted) {
			// Code to execute when the montage ends
			UE_LOG(LogTemp, Log, TEXT("Execute MontageEndedDelegate"));

			if (::IsValid(&OwnerComp) == false) return;
			if (UStateComponent* StateComponent = ControlledPawn->GetComponentByClass<UStateComponent>()) {
				StateComponent->ClearState();
			}
			if (UBlackboardComponent* LambdaBBComp = OwnerComp.GetBlackboardComponent()) {
				LambdaBBComp->SetValueAsBool(BlackboardbIsAttacking.SelectedKeyName, false);
			}
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		});
		BBComp->SetValueAsBool(BlackboardbIsAttacking.SelectedKeyName, true);
		// Execute attack
		CombatInterface->PerformAttack(AttackTypeTag, MontageEndedDelegate);
		return EBTNodeResult::InProgress;
	}
	return EBTNodeResult::Failed;
}
