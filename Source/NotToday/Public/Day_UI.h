// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Day_UI.generated.h"




UCLASS()
class NOTTODAY_API UDay_UI : public UUserWidget
{
	GENERATED_BODY()
	
private:
	virtual void NativeConstruct() override;

public:
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	class UTextBlock* StoreText;

	UPROPERTY(EditAnywhere, meta=(BindWidget))
	class UTextBlock* StoreDataText;

	UPROPERTY(EditAnywhere, meta=(BindWidget))
	class UTextBlock* PriceText;

	UPROPERTY(EditAnywhere, meta=(BindWidget))
	class UTextBlock* PriceDataText;

	UPROPERTY(EditAnywhere, meta=(BindWidget))
	class UTextBlock* CashText;

	UPROPERTY(EditAnywhere, meta=(BindWidget))
	class UTextBlock* CashDataText;

	UPROPERTY(EditAnywhere, meta=(BindWidget))
	class UTextBlock* PlaceText;

	UPROPERTY(EditAnywhere, meta=(BindWidget))
	class UTextBlock* BuyText;

	UPROPERTY(EditAnywhere, meta=(BindWidget))
	class UTextBlock* NextLevelText;

	UPROPERTY(EditAnywhere, meta=(BindWidget))
	class UTextBlock* BarricadeText;

	UPROPERTY(EditAnywhere, meta=(BindWidget))
	class UTextBlock* TurretText;

	//UPROPERTY(EditAnywhere, meta=(BindWidget))
	//class UTextBlock* Remove;

	UPROPERTY(EditAnywhere, meta=(BindWidget))
	class UButton* Button_Place;
	
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	class UButton* Button_Buy;

	UPROPERTY(EditAnywhere, meta=(BindWidget))
	class UButton* Button_NextLevel;


	UFUNCTION()
	void Place();

	UFUNCTION()
	void Buy();

	TSubclassOf<class UUserWidget> NightWidget;

	UFUNCTION()
	void NextLevel();

	class AMainPlayer* player;
	class AMainGameModeBase* GameMode;

	bool ObjectType = true;

public:
	void AnimateTextBoxPositions( float Duration , bool bMoveLeft );

	void UpdateTextBoxPosition( UTextBlock* TextBox , FVector2D StartPosition , FVector2D EndPosition , float Alpha );
	
	float EaseOutQuint( float x );

	FTimerHandle TimerHandle;
};
