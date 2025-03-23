// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MainGameModeBase.generated.h"

/**
 * 
 */

UCLASS()
class NOTTODAY_API AMainGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUserWidget> DayWidget;

	class UDay_UI* Day_UI;

	int32 CurrentScore = 0;

	void PrintStore();
	void PrintCash();
	void PrintPrice();
};
