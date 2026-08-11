// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DemoEndWidget.generated.h"

class UTextBlock;
class UButtonWidget;
/**
 * 
 */
UCLASS()
class PROJECTA_API UDemoEndWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<UTextBlock> Text;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<UButtonWidget> CloseButton;
	
public:
	UDemoEndWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void NativeConstruct() override;

protected:
	UFUNCTION()
	void OnCloseButtonClicked();

};
