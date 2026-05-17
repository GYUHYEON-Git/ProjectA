// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

struct FInputActionValue;
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class UAttributeComponent;
class UStateComponent;
class UCombatComponent;
class UTargetingComponent;
class UPlayerHUDWidget;

UCLASS()
class PROJECTA_API APlayerCharacter : public ACharacter	
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> Camera;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	// InputAction
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> SprintRollingAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> InteractAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> ToggleCombatAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> AttackAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> HeavyAttackAction;

	/** LockedOn */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> LockOnTargetAction;

	/** 왼쪽으로 타겟 전환 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> LeftTargetAction;

	/** 오른쪽으로 타겟 전환 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> RightTargetAction;

private:
	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAttributeComponent> AttributeComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStateComponent> StateComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCombatComponent> CombatComponent;

	/** LockedOn Targeting */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UTargetingComponent> TargetingComponent;

protected:
	// Montage
	UPROPERTY(EditAnywhere, Category = "Montage")
	TObjectPtr<UAnimMontage> RollingMontage;

protected:
	// UI
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UPlayerHUDWidget> PlayerHUDWidgetClass;

	UPROPERTY()
	TObjectPtr<UPlayerHUDWidget> PlayerHUDWidget;

protected:
	UPROPERTY(VisibleAnywhere, Category = "Movement Data")
	float NormalSpeed = 500.f;

	UPROPERTY(VisibleAnywhere, Category = "Movement Data")
	float SprintSpeed = 1000.f;

	UPROPERTY(VisibleAnywhere, Category = "Sprinting")
	bool bSprinting = false;

// Combo Section
protected:
	/* 콤보 시퀀스 진행중 */
	bool bComboSequenceRunning = false;
	/* 콤보 입력 가능? */
	bool bCanComboInput = false;
	/* 콤보 카운터 */
	int32 ComboCounter = 0;
	/* 콤보 입력 여부 */
	bool bSavedComboInput = false;
	/* 콤보 리셋 타이머 핸들 */
	FTimerHandle ComboResetTimerHandle;

public:
	APlayerCharacter();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void NotifyControllerChanged() override;

	void Move(const FInputActionValue& Values);

	void Look(const FInputActionValue& Values);

	FORCEINLINE UStateComponent* GetStateComponent() const { return StateComponent; }

protected:
	bool IsMoving() const;
	bool CanToggleCombat() const;
	FORCEINLINE bool IsSprinting() const { return bSprinting; }

	void Sprinting();

	void StopSprint();

	void Rolling();

	void Interact();

	void ToggleCombat();
	void AutoToggleCombat();
	/* Attack */
	void Attack();
	void SpecialAttack();
	void HeavyAttack();
	/* Lock On */
	void LockOnTarget();
	void LeftTarget();
	void RightTarget();

protected:
	FGameplayTag GetAttackPerform() const;

	/* 공격 가능 조건 체크 */
	bool CanPerformAttack(const FGameplayTag& AttackTypeTag) const;
	/* 공격 실행 */
	void DoAttack(const FGameplayTag& AttackTypeTag);
	/* 콤보 실행 */
	void ExecuteComboAttack(const FGameplayTag& AttackTypeTag);
	/* 콤보 초기화 */
	void ResetCombo();

public:
	void EnableComboWindow();
	void DisableComboWindow();
	void AttackFinished(const float ComboResetDelay);

};