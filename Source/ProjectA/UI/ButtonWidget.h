// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ButtonWidget.generated.h"

class UButton;
class UTextBlock;
/**
 * 
 */
UCLASS()
class PROJECTA_API UButtonWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<UButton> Button;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<UTextBlock> Text;

public:
	UButton* GetButton() const { return Button; }

	void SetText(FText text, float size);

public:
	UButtonWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void NativePreConstruct() override;
	
};
