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
				FGameplayTagContainer CheckTags;
				CheckTags.AddTag(MyGameplayTags::Character_State_Parried);
				CheckTags.AddTag(MyGameplayTags::Character_State_Stunned);
				if (StateComponent->IsCurrentStateEqualToAny(CheckTags) == false) {
					StateComponent->ClearState();
				}
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

EBTNodeResult::Type UBTTask_PerformAttack::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) {
	// 스턴 등으로 인해 공격 Task가 중간에 강제 취소되었을 때 실행됨
	if (UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent()) {
		// 묶여있던 공격 상태 플래그를 강제로 false로 해제!
		BBComp->SetValueAsBool(BlackboardbIsAttacking.SelectedKeyName, false);
	}

	UE_LOG(LogTemp, Warning, TEXT("PerformAttack Task was Aborted! Resetting bIsAttacking to false."));

	return Super::AbortTask(OwnerComp, NodeMemory);
}