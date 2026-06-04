// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/Targeting.h"
#include "GameFramework/Character.h"
#include "EnemyCharacter.generated.h"

class UWidgetComponent;
class USphereComponent;
class UAttributeComponent;
class UStateComponent;
class ATargetPoint;

UCLASS()
class PROJECTA_API AEnemyCharacter : public ACharacter, public ITargeting
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

	/** LockOn UI Widget */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UWidgetComponent> LockOnWidgetComponent;

// Effect Section
protected:
	UPROPERTY(EditAnywhere, Category = "Effect")
	TObjectPtr<USoundCue> ImpactSound;

	UPROPERTY(EditAnywhere, Category = "Effect")
	TObjectPtr<UParticleSystem> ImpactParticle;

// Montage Section
protected:
	UPROPERTY(EditAnywhere, Category = "Montage | HitReact")
	TObjectPtr<UAnimMontage> HitReactAnimFront;

	UPROPERTY(EditAnywhere, Category = "Montage | HitReact")
	TObjectPtr<UAnimMontage> HitReactAnimBack;

	UPROPERTY(EditAnywhere, Category = "Montage | HitReact")
	TObjectPtr<UAnimMontage> HitReactAnimLeft;

	UPROPERTY(EditAnywhere, Category = "Montage | HitReact")
	TObjectPtr<UAnimMontage> HitReactAnimRight;

protected:
	UPROPERTY(EditAnywhere, Category = "AI | Patrol")
	TArray<TObjectPtr<ATargetPoint>> PatrolPoints;

	UPROPERTY(VisibleAnywhere, Category = "AI | Patrol")
	int32 PatrolIndex = 0;

public:
	AEnemyCharacter();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float Damage, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	virtual void OnDeath();

protected:
	void ImpactEffect(const FVector& Location);
	void HitReaction(const AActor* Attacker);
	UAnimMontage* GetHitReactAnimation(const AActor* Attacker) const;

public:
	// ITargeting 
	// Targeting Logic
	virtual void OnTargeted(bool bTargeted) override;
	// Check can be targeted
	virtual bool CanBeTargeted() override;

public:
	FORCEINLINE ATargetPoint* GetPatrolPoint(){
		return PatrolPoints.Num() >= (PatrolIndex + 1) ? PatrolPoints[PatrolIndex] : nullptr;
	}
	FORCEINLINE void IncrementPatrolIndex() {
		PatrolIndex = (PatrolIndex + 1) % PatrolPoints.Num();
	}

};
