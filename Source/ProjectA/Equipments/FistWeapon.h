// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Equipments/Weapon.h"
#include "FistWeapon.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTA_API AFistWeapon : public AWeapon
{
	GENERATED_BODY()

public:
	AFistWeapon();

public:
	virtual void EquipItem() override;
	
};
