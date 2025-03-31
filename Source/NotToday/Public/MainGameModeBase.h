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

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUserWidget> NightWidget;


	class AMainPlayer* player;
	class UDay_UI* Day_UI;
	class UNight_UI* Night_UI;

	void PrintStore();
	void PrintCash();
	void PrintScore();
	void PrintPrice();
	void PrintPlace();
	void PrintRemove();
	void PrintElse();

	UFUNCTION()
	void EndGame();
	
	class AMainGameModeBase* GameMode;
	// 체력 시스템
	void SetHP( float Cur , float Max );
	void SetReload( float Cur , float Max );

	class UNight_UI* NightUIInstance;
	class UDay_UI* DayUIInstance;
};
