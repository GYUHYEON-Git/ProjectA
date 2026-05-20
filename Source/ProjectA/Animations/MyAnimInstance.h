// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Define.h"
#include "Animation/AnimInstance.h"
#include "MyAnimInstance.generated.h"

class UCharacterMovementComponent;

/**
 * 
 */
UCLASS()
class PROJECTA_API UMyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "References")
	TObjectPtr<ACharacter> Character;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "References")
	TObjectPtr<UCharacterMovementComponent> MovementComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement Data")
	FVector Velocity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement Data")
	float Speed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement Data")
	bool bShouldMove;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement Data")
	bool bIsFalling;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement Data")
	float Direction;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Data")
	bool bCombatEnabled = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Data")
	ECombatType CombatType = ECombatType::None;

public:
	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UFUNCTION()
	void AnimNotify_ResetMovementInput();

	UFUNCTION()
	void AnimNotify_ResetState();

public:
	// Animation
	void UpdateCombatMode(const ECombatType InCombatType);

protected:
	// Delegate functions
	void OnChangedCombat(const bool bInCombatEnabled);
};
