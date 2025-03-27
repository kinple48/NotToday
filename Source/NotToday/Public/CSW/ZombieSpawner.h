// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ZombieSpawner.generated.h"

class AZombieBase;
class AZombieNavVolume;

UCLASS()
class NOTTODAY_API AZombieSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	AZombieSpawner();

protected:
	virtual void BeginPlay() override;

public:	
	UPROPERTY(VisibleAnywhere)
	FVector CenterPos {FVector(0)};

	UPROPERTY(VisibleAnywhere)
	float Radius {0.f};

	UPROPERTY()
	TObjectPtr<AZombieNavVolume> NavVolume {nullptr};

	FVector RandomPosInCircle();

	UFUNCTION()
	void SpawnZombie();
	FTimerHandle SpawnTimerHandle;
	
	TArray<TPair<TSubclassOf<AZombieBase>, int32>> ZombiesArr;
	
	UPROPERTY(VisibleAnywhere)
	int32 LeftToSpawn {0};
	
	void ParseZombiesStringAndCount(const FString& Zombies, int& OutLeftToKill);

	// 스폰 시간 범위 (최소, 최대)
	UPROPERTY(EditAnywhere, Category = "Spawn")
	float MinSpawnTime = 0.5f;

	UPROPERTY(EditAnywhere, Category = "Spawn")
	float MaxSpawnTime = 2.0f;
};
