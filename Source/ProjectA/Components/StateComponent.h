// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "StateComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTA_API UStateComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State");
	bool bMovementInputEnabled = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	FGameplayTag CurrentState;

public:	
	UStateComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	FORCEINLINE bool MovementInputEnabled() const { return bMovementInputEnabled; }
	FORCEINLINE void SetState(const FGameplayTag NewState) { CurrentState = NewState; }
	FORCEINLINE FGameplayTag GetCurrentState() const { return CurrentState; }

	void ClearState();

	bool IsCrrentStateEqualToAny(const FGameplayTagContainer& TagsToCheck) const;

	void ToggleMovementInput(bool bEnabled, float Duration = 0.f);

protected:
	UFUNCTION()
	void MovementInputEnableAction();

};
