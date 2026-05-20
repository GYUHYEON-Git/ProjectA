// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Define.h"
#include "Components/ActorComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "TargetingComponent.generated.h"

class UCameraComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTA_API UTargetingComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	/** Radius of the area used to detect LockOn candidates */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float TargetingRadius = 500.f;

	/** Camera rotation speed while LockOn is active */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float FaceLockOnRotationSpeed = 20.f;

	/** Debug draw type */
	UPROPERTY(EditAnywhere)
	TEnumAsByte<EDrawDebugTrace::Type> DrawDebugType = EDrawDebugTrace::ForDuration;

protected:
	UPROPERTY()
	TObjectPtr<ACharacter> Character;

	UPROPERTY()
	TObjectPtr<UCameraComponent> Camera;

	/** Currently locked-on target */
	UPROPERTY()
	TObjectPtr<AActor> LockedTargetActor;

	/** Tracks the LockOn state */
	bool bIsLockOn = false;

public:	
	UTargetingComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	/** LockedOn On/Off */
	void ToggleLockOn();

	/** Switch the current LockOn target */
	void SwitchingLockedOnActor(ESwitchingDirection InDirection);

	/** Current LockOn state */
	FORCEINLINE bool IsLockOn() const { return bIsLockOn; }

protected:
	/** Find potential LockOn candidates */
	void FindTargets(TArray<AActor*>& OutTargetingActors) const;

	/** Select the final LockOn target */
	AActor* FindClosestTarget(TArray<AActor*>& InTargets, ESwitchingDirection InDirection = ESwitchingDirection::None) const;

	/** Make the character follow the camera (ControlRotation) */
	void OrientCamera() const;

	/** Make the character rotate toward the movement direction */
	void OrientMovement() const;

	/** Control camera rotation while LockedOn */
	void FaceLockOnActor() const;

	/** LockOn */
	void LockOnTarget();

	/** Stop LockOn */
	void StopLockOn();

		
};
