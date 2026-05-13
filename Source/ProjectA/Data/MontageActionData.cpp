// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/MontageActionData.h"

UAnimMontage* UMontageActionData::GetMontageForTag(const FGameplayTag& GroupTag, const int32 Index) {
	if (FMontageGroup* MontageGroup = MontageGroupMap.Find(GroupTag)) {
		if (MontageGroup->Animations.IsValidIndex(Index)) {
			return MontageGroup->Animations[Index];
		}
	}
	return nullptr;
}