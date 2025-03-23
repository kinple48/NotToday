// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Night_UI.generated.h"

/**
 * 
 */
UCLASS()
class NOTTODAY_API UNight_UI : public UUserWidget
{
	GENERATED_BODY()
private:
	virtual void NativeConstruct() override;

public:
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	class UTextBlock* ScoreText;

	UPROPERTY(EditAnywhere, meta=(BindWidget))
	class UTextBlock* ScoreDataText;

	UPROPERTY(EditAnywhere, meta=(BindWidget))
	class UTextBlock* HealthText;

	UPROPERTY(EditAnywhere, meta=(BindWidget))
	class UTextBlock* ReloadText;

	UPROPERTY(EditAnywhere, meta=(BindWidget))
	class UTextBlock* CashText;

	UPROPERTY(EditAnywhere, meta=(BindWidget))
	class UTextBlock* CashDataText;

	UPROPERTY(EditAnywhere, meta=(BindWidget))
	class UProgressBar* HPBar;

	UPROPERTY(EditAnywhere, meta=(BindWidget))
	class UProgressBar* ReloadBar;

	class AMainPlayer* player;
	class AMainGameModeBase* GameMode;

	void SetHP( float Cur , float Max );
	void SetReload( float Cur , float Max );
};
