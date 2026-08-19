// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Define.h"
#include "UObject/Interface.h"
#include "CombatInterface.generated.h"

struct FGameplayTag;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UCombatInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROJECTA_API ICombatInterface
{
	GENERATED_BODY()

public:
	virtual void ActivateWeaponCollision(EWeaponCollisionType WeaponCollisionType) = 0;
	virtual void DeactivateWeaponCollision(EWeaponCollisionType WeaponCollisionType) = 0;
	virtual void PerformAttack(FGameplayTag& AttackTypeTag, FOnMontageEnded& MontageEndedDelegate){}
	virtual void Parried() {}
	virtual void ToggleIFrames(const bool bEnabled) {}

};
