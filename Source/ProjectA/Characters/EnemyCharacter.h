// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/Targeting.h"
#include "Interfaces/CombatInterface.h"
#include "GameFramework/Character.h"
#include "EnemyCharacter.generated.h"

class UWidgetComponent;
class USphereComponent;
class UAttributeComponent;
class UStateComponent;
class UCombatComponent;
class URotationComponent;
class ATargetPoint;
class AWeapon;

UCLASS()
class PROJECTA_API AEnemyCharacter : public ACharacter, public ITargeting, public ICombatInterface
{
	GENERATED_BODY()

protected:
	// Check Targeting
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> TargetingSphereComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAttributeComponent> AttributeComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStateComponent> StateComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCombatComponent> CombatComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<URotationComponent> RotationComponent;

	/** LockOn UI Widget */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UWidgetComponent> LockOnWidgetComponent;

	/** HealthBar */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UWidgetComponent> HealthBarWidgetComponent;

// Effect Section
protected:
	UPROPERTY(EditAnywhere, Category = "Effect")
	TObjectPtr<USoundCue> ImpactSound;

	UPROPERTY(EditAnywhere, Category = "Effect")
	TObjectPtr<UParticleSystem> ImpactParticle;

protected:
	UPROPERTY(EditAnywhere, Category = "AI | Patrol")
	TArray<TObjectPtr<ATargetPoint>> PatrolPoints;

	UPROPERTY(VisibleAnywhere, Category = "AI | Patrol")
	int32 PatrolIndex = 0;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AWeapon> DefaultWeaponClass;

public:
	AEnemyCharacter();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float Damage, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	virtual void OnDeath();
	void OnAttributeChanged(EAttributeType AttributeType, float InValue);
	void SetupHealthBar();

protected:
	void ImpactEffect(const FVector& Location);
	void HitReaction(const AActor* Attacker);

public:
	// ITargeting 
	// Targeting Logic
	virtual void OnTargeted(bool bTargeted) override;
	// Check can be targeted
	virtual bool CanBeTargeted() override;

	// ICombatInterface 구현.
	virtual void ActivateWeaponCollision(EWeaponCollisionType WeaponCollisionType) override;
	virtual void DeactivateWeaponCollision(EWeaponCollisionType WeaponCollisionType) override;
	virtual void PerformAttack(FGameplayTag& AttackTypeTag, FOnMontageEnded& MontageEndedDelegate) override;

	// 체력바 토글
	void ToggleHealthBarVisibility(bool bVisibility);

public:
	FORCEINLINE ATargetPoint* GetPatrolPoint(){
		return PatrolPoints.Num() >= (PatrolIndex + 1) ? PatrolPoints[PatrolIndex] : nullptr;
	}
	FORCEINLINE void IncrementPatrolIndex() {
		PatrolIndex = (PatrolIndex + 1) % PatrolPoints.Num();
	}

};