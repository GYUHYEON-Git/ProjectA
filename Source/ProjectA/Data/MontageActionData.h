// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "MontageActionData.generated.h"

USTRUCT(BlueprintType)
struct FMontageGroup {
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TObjectPtr<UAnimMontage>> Animations;
};

/**
 * 
 */


UCLASS()
class PROJECTA_API UMontageActionData : public UPrimaryDataAsset
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName = "Montage Groups")
	TMap<FGameplayTag, FMontageGroup> MontageGroupMap;
	
public:
	UAnimMontage* GetMontageForTag(const FGameplayTag& GroupTag, const int32 Index);
	bool HasValidMontage(const FGameplayTag& GroupTag);
};
