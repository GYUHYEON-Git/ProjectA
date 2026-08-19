// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Define.h"
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
	ECombatType CombatType = ECombatType::SwordShield;

	// Manages animations as Data Assets to play different animations based on the equipped weapon.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment | Animation")
	TObjectPtr<UMontageActionData> MontageActionData;

protected:	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UWeaponCollisionComponent> WeaponCollision;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UWeaponCollisionComponent> SecondWeaponCollision;

protected:
	UPROPERTY()
	TObjectPtr<UCombatComponent> CombatComponent;

protected:
	UPROPERTY(EditAnywhere)
	TMap<FGameplayTag, float> StaminaCostMap;

	UPROPERTY(EditAnywhere)
	float BaseDamage = 15.f;

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
	UAnimMontage* GetRandomMontageForTag(const FGameplayTag& Tag) const;
	UAnimMontage* GetHitReactMontage(const AActor* Attacker) const;

	bool HasValidMontage(const FGameplayTag& Tag) const;

	FORCEINLINE FName GetEquipSocketName() const { return EquipSocketName; }
	FORCEINLINE FName GetUnequipSocketName() const { return UnequipSocketName; }
	FORCEINLINE UWeaponCollisionComponent* GetCollision() const { return WeaponCollision; }
	FORCEINLINE ECombatType GetCombatType() const { return CombatType; }

public:
	virtual void ActivateCollision(EWeaponCollisionType InCollisionType);
	virtual void DeactivateCollision(EWeaponCollisionType InCollisionType);

public:
	void OnHitActor(const FHitResult& Hit);
};
