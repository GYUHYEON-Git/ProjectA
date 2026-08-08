// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Equipments/Equipment.h"
#include "Shield.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTA_API AShield : public AEquipment
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName EquipSocketName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName UnequipSocketName;

public:
	virtual void EquipItem() override;

	FORCEINLINE FName GetEquipSocketName() const { return EquipSocketName; }
	FORCEINLINE FName GetUnequipSocketName() const { return UnequipSocketName; }
	
};
