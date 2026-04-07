// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StatBarWidget.generated.h"

class UProgressBar;
/**
 * 
 */
UCLASS()
class PROJECTA_API UStatBarWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<UProgressBar> StatBar;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor FillColorAndOpacity = FLinearColor::Green;

public:
	UStatBarWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void NativePreConstruct() override;

	void SetRatio(float Ratio) const;
	
};
