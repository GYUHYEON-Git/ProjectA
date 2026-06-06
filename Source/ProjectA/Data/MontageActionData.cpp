// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/MontageActionData.h"

UAnimMontage* UMontageActionData::GetMontageForTag(const FGameplayTag& GroupTag, const int32 Index) const {
	if (const FMontageGroup* MontageGroup = MontageGroupMap.Find(GroupTag)) {
		if (MontageGroup->Animations.IsValidIndex(Index)) {
			return MontageGroup->Animations[Index];
		}
	}
	return nullptr;
}

UAnimMontage* UMontageActionData::GetRandomMontageForTag(const FGameplayTag& GroupTag) const {
	if (const FMontageGroup* MontageGroup = MontageGroupMap.Find(GroupTag)) {
		// ·£´ý Index
		const int32 RandomIndex = FMath::RandRange(0, MontageGroup->Animations.Num() - 1);
		return MontageGroup->Animations[RandomIndex];
	}
	return nullptr;
}

bool UMontageActionData::HasValidMontage(const FGameplayTag& GroupTag) {
	if (FMontageGroup* MontageGroup = MontageGroupMap.Find(GroupTag)) {
		return true;
	}
	return false;
}
