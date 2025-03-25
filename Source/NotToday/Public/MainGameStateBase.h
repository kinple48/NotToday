// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "MainGameStateBase.generated.h"

/**
 * 
 */

class AZombieSpawner;

UENUM(BlueprintType)
enum class EDayNightState : uint8
{
	Day,
	Night,
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDayNightChanged, EDayNightState, NewState);

UCLASS()
class NOTTODAY_API AMainGameStateBase : public AGameStateBase
{
	GENERATED_BODY()
public:
	FOnDayNightChanged OnDayNightChanged;

	UFUNCTION()
	void SetDayNightState(EDayNightState NewState);

protected:
	virtual void BeginPlay() override;

private:
	EDayNightState State;

	FTimerHandle NightTimer; // 좀비가 모두 죽을 때까지 계속해서 확인한다.

	UFUNCTION()
	void CheckAllKill();

	int32 CurrentWave {1}; // 현재 웨이브

	TArray<AZombieSpawner> ZombieSpawners; // 레벨에 있는 모든 좀비 스폰지점
};
