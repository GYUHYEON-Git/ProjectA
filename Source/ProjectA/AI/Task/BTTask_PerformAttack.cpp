// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Task/BTTask_PerformAttack.h"

#include "AIController.h"
#include "Components/StateComponent.h"
#include "Interfaces/CombatInterface.h"


EBTNodeResult::Type UBTTask_PerformAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) {
	APawn* ControlledPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (!ControlledPawn) {
		return EBTNodeResult::Failed;
	}
	if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(ControlledPawn)) {
		FOnMontageEnded MontageEndedDelegate;
		// 델리게이트 바인딩
		MontageEndedDelegate.BindLambda([this, &OwnerComp, ControlledPawn](UAnimMontage* Montage, bool bInterrupted) {
			// 몽타주 종료 시 실행될 코드
			UE_LOG(LogTemp, Log, TEXT("Execute MontageEndedDelegate"));

			if (::IsValid(&OwnerComp) == false) return;
			if (UStateComponent* StateComponent = ControlledPawn->GetComponentByClass<UStateComponent>()) {
				StateComponent->ClearState();
			}
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, TEXT("Succeeded"));
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		});
		// Attack 실행
		CombatInterface->PerformAttack(AttackTypeTag, MontageEndedDelegate);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, TEXT("InProgress"));
		return EBTNodeResult::InProgress;
	}
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, TEXT("Failed"));
	return EBTNodeResult::Failed;
}
