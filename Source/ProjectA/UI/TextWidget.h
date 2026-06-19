// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TextWidget.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class PROJECTA_API UTextWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<UTextBlock> TextBox;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Text;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 FontSize = 24;

public:
	UTextWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void NativePreConstruct() override;

	void SetTextBlock(FString EquipmentName);

};
