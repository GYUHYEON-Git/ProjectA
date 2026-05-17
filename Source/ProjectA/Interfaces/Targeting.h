// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Targeting.generated.h"

// This class does not need to be modified.
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

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void OnTargeted(bool bTargeted) = 0;

	virtual bool CanBeTargeted() = 0;
};
