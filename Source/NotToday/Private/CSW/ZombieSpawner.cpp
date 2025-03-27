// Fill out your copyright notice in the Description page of Project Settings.


#include "CSW/ZombieSpawner.h"

#include "EngineUtils.h"
#include "MainGameStateBase.h"
#include "CSW/ZombieBase.h"
#include "CSW/ZombieNavVolume.h"
#include "NotToday/NotToday.h"

class AMainGameStateBase;
// Sets default values
AZombieSpawner::AZombieSpawner()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AZombieSpawner::BeginPlay()
{
	Super::BeginPlay();

	// 레벨에 배치된 NavVolume을 가져온다.
	NavVolume = TActorIterator<AZombieNavVolume>(GetWorld()).operator*();
	if (!NavVolume)
	{
		UE_LOG(LogTemp, Error, TEXT("Nav Volume == nullptr !!"));
		return;
	}
	FVector Extent = NavVolume->GetBounds().BoxExtent;
	Radius = FMath::Min(Extent.X, Extent.Y);

	CenterPos = NavVolume->GetActorLocation();
	PRINT_LOG(CSW, TEXT("AZombieSpawner::BeginPlay"));

	// 델리게이트 바인딩
	auto gs = Cast<AMainGameStateBase>(GetWorld()->GetGameState());
	if (!gs)
	{
		PRINT_LOG(CSW, TEXT("MainGameStateBase is nullptr"));
	}
	gs->OnNightStarted.AddDynamic(this, &AZombieSpawner::SpawnZombie);
}

FVector AZombieSpawner::RandomPosInCircle()
{
	float RandTheta = FMath::RandRange(0.f, 2.f * PI);
	float X = CenterPos.X + Radius * FMath::Cos(RandTheta);
	float Y = CenterPos.Y + Radius * FMath::Sin(RandTheta);
	return FVector(X, Y, CenterPos.Z + 50.f);
}

// 스폰해야할 좀비들을 모두 스폰할 때까지 좀비 스폰 타이머를 돌린다.
void AZombieSpawner::SpawnZombie()
{
	if (ZombiesArr.Num() == 0) return;
	
	int RandomIdx = FMath::RandRange(0, ZombiesArr.Num() - 1);
	
	int& Count = ZombiesArr[RandomIdx].Value;
	
	if (Count == 0)
	{
		// 이미 다 소환한 좀비클래스면, 바로 다른 것을 소환한다
		SpawnZombie();
	}
	else
	{
		PRINT_LOG(CSW, TEXT("SpawnZombie"));

		FVector Pos = RandomPosInCircle();
		TSubclassOf<AZombieBase> ZombieClass = ZombiesArr[RandomIdx].Key;
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		auto SpawnedZombie = GetWorld()->SpawnActor<AZombieBase>(ZombieClass, Pos, FRotator::ZeroRotator, SpawnParams);
		if (SpawnedZombie)
		{
			// SpawnActor로 만들어줬을 때는, 이렇게 직접 Controller를 넣어줘야한다!!
			SpawnedZombie->SpawnDefaultController(); 
		}
		Count--;
		LeftToSpawn--;

		if (LeftToSpawn <= 0)
		{
			return;
		}

		// 다음 스폰까지 랜덤 시간 설정
		float RandomSpawnTime = FMath::RandRange(MinSpawnTime, MaxSpawnTime);
		GetWorld()->GetTimerManager().SetTimer(SpawnTimerHandle, this, &AZombieSpawner::SpawnZombie, RandomSpawnTime, false);
	}
}

void AZombieSpawner::ParseZombiesStringAndCount(const FString& Zombies, int& OutLeftToKill)
{
	int32 LeftZombieNum = 0;
	TArray<TPair<TSubclassOf<AZombieBase>, int32>> Temp;
	TArray<FString> ZombiePairs;
	Zombies.ParseIntoArray(ZombiePairs, TEXT(","), true);

	for (const FString& Pair: ZombiePairs)
	{
		// :로 분리
		TArray<FString> PairParts; // [좀비종류, 마리 수]
		Pair.TrimStartAndEnd().ParseIntoArray(PairParts, TEXT(":"), true);

		if (PairParts.Num() == 2)
		{
			FString Zombie = PairParts[0].TrimStartAndEnd();
			int32 Count = FCString::Atoi(*PairParts[1]); // FString -> int32

			FString ZombiePath = FString::Printf(TEXT("/Game/CSW/Blueprints/Zombie/BP_%s.BP_%s_C"), *Zombie, *Zombie);

			TSubclassOf<AZombieBase> ZombieClass = LoadClass<AZombieBase>(nullptr, *ZombiePath);

			if (!ZombieClass)
			{
				PRINT_LOG(CSW, TEXT("ZombieClass == nullptr.."));
			}
			Temp.Add(TPair<TSubclassOf<AZombieBase>, int32>(ZombieClass, Count));
			LeftZombieNum += Count;
		}
	}
	PRINT_LOG(CSW, TEXT("LeftZombieNum = %d"), LeftZombieNum);
	ZombiesArr = Temp;
	LeftToSpawn = LeftZombieNum;
	OutLeftToKill = LeftZombieNum;
}
