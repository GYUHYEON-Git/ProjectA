// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Define.h"
#include "BehaviorTree/BTService.h"
#include "BTService_SelectBehavior.generated.h"

class AEnemyCharacter;
/**
 * 
 */
UCLASS()
class PROJECTA_API UBTService_SelectBehavior : public UBTService
{
	GENERATED_BODY()

protected:
	UPROPERTY()
	TObjectPtr<AEnemyCharacter> ControlledEnemy;

	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector TargetKey;

	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector BehaviorKey;

	UPROPERTY(EditAnywhere)
	float AttackRangeDistance = 500.f;

public:
	UBTService_SelectBehavior();

protected:
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	void SetBehaviorKey(UBlackboardComponent* BlackboardComp, EAIBehavior Behavior) const;
	void UpdateBehavior(UBlackboardComponent* BlackboardComp) const;

	
};
