// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/CombatInterface.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

struct FInputActionValue;
class AFistWeapon;
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
class PROJECTA_API APlayerCharacter : public ACharacter, public ICombatInterface
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

	/** Switch target to the left. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> LeftTargetAction;

	/** Switch target to the right. */
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
	// Fist weapon
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AFistWeapon> FistWeaponClass;

protected:
	UPROPERTY(EditAnywhere, Category = "Effect")
	TObjectPtr<USoundCue> ImpactSound;

	UPROPERTY(EditAnywhere, Category = "Effect")
	TObjectPtr<UParticleSystem> ImpactParticle;

protected:
	UPROPERTY(VisibleAnywhere, Category = "Movement Data")
	float NormalSpeed = 500.f;

	UPROPERTY(VisibleAnywhere, Category = "Movement Data")
	float SprintSpeed = 1000.f;

	UPROPERTY(VisibleAnywhere, Category = "Sprinting")
	bool bSprinting = false;

// Combo Section
protected:
	/* Is the combo sequence in progress? */
	bool bComboSequenceRunning = false;
	/* Can the player input the next combo attack? */
	bool bCanComboInput = false;
	/* Current combo count. */
	int32 ComboCounter = 0;
	/* Whether combo input has been received. */
	bool bSavedComboInput = false;
	/* Timer handle used to reset the combo. */
	FTimerHandle ComboResetTimerHandle;

public:
	APlayerCharacter();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void NotifyControllerChanged() override;

public:
	FORCEINLINE UStateComponent* GetStateComponent() const { return StateComponent; }
	bool IsDeath() const;
	
	virtual float TakeDamage(float Damage, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	void ImpactEffect(const FVector& Location);
	void HitReaction(const AActor* Attacker);
	void OnDeath();

protected:
	bool IsMoving() const;
	bool CanToggleCombat() const;
	bool CanRolling() const;
	FORCEINLINE bool IsSprinting() const { return bSprinting; }

	void Move(const FInputActionValue& Values);

	void Look(const FInputActionValue& Values);

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

	/* Check whether an attack can be performed. */
	bool CanPerformAttack(const FGameplayTag& AttackTypeTag) const;
	/* Execute the attack. */
	void DoAttack(const FGameplayTag& AttackTypeTag);
	/* Execute the combo. */
	void ExecuteComboAttack(const FGameplayTag& AttackTypeTag);
	/* Reset the combo. */
	void ResetCombo();

public:
	void EnableComboWindow();
	void DisableComboWindow();
	void AttackFinished(const float ComboResetDelay);

public:
	virtual void ActivateWeaponCollision(EWeaponCollisionType WeaponCollisionType) override;
	virtual void DeactivateWeaponCollision(EWeaponCollisionType WeaponCollisionType) override;


};