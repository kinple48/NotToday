// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CSW/WaveTableRow.h"
#include "CSW/ZombieBase.h"
#include "GameFramework/GameStateBase.h"
#include "MainGameStateBase.generated.h"

/**
 * 
 */

struct FWaveTableRow;
class AZombieSpawner;

UENUM(BlueprintType)
enum class EDayNightState : uint8
{
	Day,
	Night,
};

// DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDayNightChanged, EDayNightState, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDayStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnNightStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnZombieKilled, AZombieBase*, KilledZombie);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameClear);

UCLASS()
class NOTTODAY_API AMainGameStateBase : public AGameStateBase
{
	GENERATED_BODY()
public:
	FOnDayStarted OnDayStarted;
	FOnNightStarted OnNightStarted;
	
	FOnZombieKilled OnZombieKilled; // 좀비가 죽으면 ZombieKilled 함수에 의해 브로드캐스트 된다.
	FOnGameClear OnGameClear;

	UFUNCTION()
	void SetDayNightState(EDayNightState NewState);
	UFUNCTION()
	void ZombieKilled(AZombieBase* Zombie);

protected:
	virtual void BeginPlay() override;
	
private:
	UPROPERTY(VisibleAnywhere)
	EDayNightState State;
	
	FTimerHandle CheckAllKillTimerHandle; // 좀비가 모두 죽을 때까지 계속해서 확인한다.
	
	UFUNCTION()
	void DecreaseAndCheckLeftToKill(AZombieBase* Zombie);
	void CheckLeftZombieCount();

public:
	UFUNCTION()
	void LoadNextWaveData();

	UFUNCTION()
	void PlayWaveStartSound();
	
private:
	// 웨이브 데이터
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UDataTable> WaveDataTable {nullptr};
	
	UPROPERTY(VisibleAnywhere)
	int32 NextWaveID {0};
	
	UPROPERTY(VisibleAnywhere)
	int32 Money;
	
	UPROPERTY(VisibleAnywhere)
	int32 LeftToKill {0};

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<AZombieSpawner> ZombieSpawner;

	UPROPERTY(VisibleAnywhere)
	class AMainPlayer* player;

	UPROPERTY(VisibleAnywhere)
	class APlayerController* pc;

	// 아이템 자석 흡수
	void ItemMagnet();
	
	FTimerHandle ItemMagnetTimerHandle;
	
	float ItemElapsedTime {0.f};
	
	UPROPERTY(EditAnywhere)
	float ItemMagnetDuration { 1.5f };
};
