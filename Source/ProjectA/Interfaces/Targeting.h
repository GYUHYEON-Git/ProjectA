// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Targeting.generated.h"

UINTERFACE(MinimalAPI)
class UTargeting : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROJECTA_API ITargeting
{
	GENERATED_BODY()

public:
	virtual void OnTargeted(bool bTargeted) = 0;
	virtual bool CanBeTargeted() = 0;
};
