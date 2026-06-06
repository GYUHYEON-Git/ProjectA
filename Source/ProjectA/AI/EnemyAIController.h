// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIController.generated.h"

class AEnemyCharacter;
/**
 * 
 */
UCLASS()
class PROJECTA_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UBehaviorTree> BehaviorTreeAsset;

	/* AI가 주변환경을 인식할 수 있게 해주는 컴포넌트 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UAIPerceptionComponent> AIPerceptionComponent;

	FTimerHandle TimerHandle;

	UPROPERTY()
	TObjectPtr<AEnemyCharacter> ControlledEnemy;

public:
	AEnemyAIController();

protected:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

protected:
	void UpdateTarget() const;
	void SetTarget(AActor* NewTarget) const;
	
};
