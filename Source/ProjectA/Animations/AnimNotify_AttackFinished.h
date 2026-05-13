// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_AttackFinished.generated.h"

/**
 * 
 */
UCLASS(meta = (DisplayName = "Attack Finished"))
class PROJECTA_API UAnimNotify_AttackFinished : public UAnimNotify
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere)
	float ComboResetDelay = 1.f;

public:
	UAnimNotify_AttackFinished(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	
};
