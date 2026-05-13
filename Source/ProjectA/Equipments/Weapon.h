// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Equipments/Equipment.h"
#include "Weapon.generated.h"

class UCombatComponent;
class UMontageActionData;
class UWeaponCollisionComponent;
/**
 * 
 */
UCLASS()
class PROJECTA_API AWeapon : public AEquipment
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment | Socket")
	FName EquipSocketName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment | Socket")
	FName UnequipSocketName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment | Animation")
	TObjectPtr<UMontageActionData> MontageActionData;

// Component Section
protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UWeaponCollisionComponent> WeaponCollision;

protected:
	UPROPERTY()
	TObjectPtr<UCombatComponent> CombatComponent;

// Data Section
protected:
	UPROPERTY(EditAnywhere)
	TMap<FGameplayTag, float> StaminaCostMap;

	// Damage
	UPROPERTY(EditAnywhere)
	float BaseDamage = 15.f;

	// DamageMultiplier
	UPROPERTY(EditAnywhere)
	TMap<FGameplayTag, float> DamageMultiplierMap;

public:
	AWeapon();

public:
	virtual void EquipItem() override;

public:
	float GetStaminaCost(const FGameplayTag& InTag) const;
	float GetAttackDamage() const;

	UAnimMontage* GetMontageForTag(const FGameplayTag& Tag, const int32 Index = 0) const;

	FORCEINLINE FName GetEquipSocketName() const { return EquipSocketName; }
	FORCEINLINE FName GetUnequipSocketName() const { return UnequipSocketName; }
	FORCEINLINE UWeaponCollisionComponent* GetCollision() const { return WeaponCollision; }

public:
	void OnHitActor(const FHitResult& Hit);
};
