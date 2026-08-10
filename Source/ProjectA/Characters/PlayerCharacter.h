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
class USphereComponent;
class UPlayerHUDWidget;
class APickupItem;

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> BlockAction;

	/* 패링 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> ParryAction;



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

	// Item Overlap
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USphereComponent> SphereComponent;

protected:
	UPROPERTY()
	TArray<TObjectPtr<APickupItem>> InteractableItems;

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
	TObjectPtr<USoundBase> ImpactSound;
	
	UPROPERTY(EditAnywhere, Category = "Effect")
	TObjectPtr<USoundBase> BlockingSound;

	UPROPERTY(EditAnywhere, Category = "Effect")
	TObjectPtr<UParticleSystem> ImpactParticle;

	UPROPERTY(EditAnywhere, Category = "Effect")
	TObjectPtr<UParticleSystem> BlockingParticle;

protected:
	UPROPERTY(EditAnywhere, Category = "Movement Speed")
	float NormalSpeed = 500.f;

	UPROPERTY(EditAnywhere, Category = "Movement Speed")
	float SprintSpeed = 750.f;

	UPROPERTY(EditAnywhere, Category = "Movement Speed")
	float BlockingSpeed = 250.f;

	UPROPERTY(VisibleAnywhere, Category = "Movement Speed")
	bool bSprinting = false;

	UPROPERTY(EditAnywhere, Category = "Stamina Data")
	TMap<FName, float> StaminaMap;

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

protected:
	/* 적과 대치중인 방향인지? */
	bool bFacingEnemy = false;

protected:
	/* Whether invincibility frames are enabled */
	bool bEnabledIFrames = false;

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
	void ShieldBlockingEffect(const FVector& Location) const;
	void HitReaction(const AActor* Attacker);
	void OnDeath();

protected:
	bool IsMoving() const;
	bool CanToggleCombat() const;
	bool CanRolling() const;
	FORCEINLINE bool IsSprinting() const { return bSprinting; }
	FORCEINLINE bool CanReceiveDamage() const { return !bEnabledIFrames; }
	FORCEINLINE float GetStamina(FName FName) const { return StaminaMap[FName]; }

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
	/* 방어 자세 */
	void Blocking();
	void BlockingEnd();
	/* 패링 */
	void Parrying();

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
	/* 방어 자세 가능 여부 */
	bool CanPlayerBlockStance() const;
	/* 방패 막기 방어가 가능한지? */
	bool CanPerformAttackBlocking() const;
	/* 패링이 가능한지? */
	bool CanPerformParry() const;
	/* 패링 성공 여부 */
	bool ParriedAttackSucceed() const;

public:
	void EnableComboWindow();
	void DisableComboWindow();
	void AttackFinished(const float ComboResetDelay);

public:
	virtual void ActivateWeaponCollision(EWeaponCollisionType WeaponCollisionType) override;
	virtual void DeactivateWeaponCollision(EWeaponCollisionType WeaponCollisionType) override;
	virtual void ToggleIFrames(const bool bEnabled) override;

public:
	UPlayerHUDWidget* GetHUDWidget() const;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
		
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	AActor* GetClosestItem();

};