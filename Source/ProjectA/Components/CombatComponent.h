// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyGameplayTags.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"

class AWeapon;
class AShield;
class AEquipment;

DECLARE_MULTICAST_DELEGATE_OneParam(FDelegateOnChangedCombat, bool)

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTA_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	/* Delegate broadcast when the Combat state changes */
	FDelegateOnChangedCombat OnChangedCombat;

protected:
	UPROPERTY()
	TObjectPtr<AWeapon> MainWeapon;

	UPROPERTY()
	TObjectPtr<AShield> Shield;

	UPROPERTY(EditAnywhere)
	bool bCombatEnabled = false;

	UPROPERTY(VisibleAnywhere)
	FGameplayTag LastAttackType;

public:	
	UCombatComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	void SetWeapon(AWeapon* NewWeapon);
	void SetShield(AShield* NewShield);

	FORCEINLINE bool IsCombatEnabled() const { return bCombatEnabled; }
	void SetCombatEnabled(const bool bEnabled);

	FORCEINLINE AWeapon* GetMainWeapon() const { return MainWeapon; }
	FORCEINLINE AShield* GetShield() const { return Shield; }

	FORCEINLINE FGameplayTag GetLastAttackType() const { return LastAttackType; }
	FORCEINLINE void SetLastAttackType(const FGameplayTag& NewAttackTypeTag) { LastAttackType = NewAttackTypeTag; }
	
private:
	void SpawnPickupItem(const AActor* OwnerActor, const TSubclassOf<AEquipment>& NewEquipmentClass, const FString NewEquipmentName) const;
};
