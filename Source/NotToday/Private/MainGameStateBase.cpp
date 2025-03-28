// Fill out your copyright notice in the Description page of Project Settings.


#include "MainGameStateBase.h"

#include "EngineUtils.h"
#include "CSW/WaveTableRow.h"
#include "CSW/ZombieSpawner.h"
#include "NotToday/NotToday.h"
#include "MainPlayer.h"

void AMainGameStateBase::BeginPlay()
{
	Super::BeginPlay();
	ZombieSpawner = TActorIterator<AZombieSpawner>(GetWorld()).operator*();
	player = Cast<AMainPlayer>( GetWorld()->GetFirstPlayerController()->GetPawn() );
	if (player)
	{
		player->HP = player->HPMax;
		player->Reload = player->ReloadMax;
	}

	pc = GetWorld()->GetFirstPlayerController();
	if (pc)
	{
		pc->SetShowMouseCursor( true );
	}

	if (!ZombieSpawner)
	{
		PRINT_LOG(CSW, TEXT("ZombieSpawner == nullptr"));
	}

	// 델리게이트 바인딩
	OnDayStarted.AddDynamic(this, &AMainGameStateBase::LoadNextWaveData); // LoadNextWaveData
	
	OnZombieKilled.AddDynamic(this, &AMainGameStateBase::DecreaseAndCheckLeftToKill); // DecreaseAndCheckLeftToKill
	
	LoadNextWaveData();
	ZombieSpawner->SpawnZombie();
}

void AMainGameStateBase::SetDayNightState(EDayNightState NewState)
{
	State = NewState;

	// GameState클래스가 아닌 부분에서는
	// OnDayNightChanged.AddDynamic을 한다!!
	//OnDayNightChanged.Broadcast(State);

	// GameState 클래스 내부 처리
	if (State == EDayNightState::Day)
	{
		PRINT_LOG(CSW, TEXT("SetDayNightState --> Day"));
		// 낮
		OnDayStarted.Broadcast();
		// 다음 웨이브 데이터를 가져온다.
		//LoadNextWaveData();
	}
	else
	{
		PRINT_LOG(CSW, TEXT("SetDayNightState --> Night"));
		// 밤 ( 웨이브 시작 )
		OnNightStarted.Broadcast();
		
		//ZombieSpawner->SpawnZombie();
		//StartCheckAllKillLoop(1.f); // 좀비 수 확인 루프 시작
	}
}

void AMainGameStateBase::ZombieKilled(AZombieBase* Zombie)
{
	PRINT_LOG(CSW, TEXT("OnZombieKilled.Broadcast"));
	OnZombieKilled.Broadcast(Zombie);
}

void AMainGameStateBase::LoadNextWaveData()
{
	if (!WaveDataTable)
	{
		PRINT_LOG(CSW, TEXT("WaveDataTable == nullptr"));
		return;
	}
	NextWaveID += 1;
	FName WaveID = FName(*FString::FromInt((NextWaveID)));
	FWaveTableRow*  WaveData;
	WaveData = WaveDataTable->FindRow<FWaveTableRow>(WaveID, TEXT("LoadWaveDataById"));
	if (!WaveData)
	{
		PRINT_LOG(CSW, TEXT("WaveData == nullptr"));
		return;
	}

	if (WaveData->ID == FName(*FString::FromInt((-1))))
	{
		// 게임 클리어
		PRINT_LOG(CSW, TEXT("Game Clear !!"));
		OnGameClear.Broadcast();
		return;
	}

	// 가져온 데이터 뿌려주기
	//Zombies = WaveData->Zombies;
	Money = WaveData->Money;
	ZombieSpawner->ParseZombiesStringAndCount(WaveData->Zombies, LeftToKill);
}

// LeftZombieNum이 0이 될 때까지 CheckLeft 루프를 돈다.
void AMainGameStateBase::StartCheckAllKillLoop(float Rate)
{
	// GetWorld()->GetTimerManager().SetTimer(CheckAllKillTimerHandle,
	// 	this, &AMainGameStateBase::CheckLeftZombieCount,
	// 	Rate, true, Rate
	// );
}

void AMainGameStateBase::DecreaseAndCheckLeftToKill(AZombieBase* Zombie)
{
	LeftToKill--;
	PRINT_LOG(CSW, TEXT("Left To Kill = %d"), LeftToKill);
	CheckLeftZombieCount();
}

void AMainGameStateBase::CheckLeftZombieCount()
{
	// 0 이하면
	if (LeftToKill == 0)
	{
		//GetWorld()->GetTimerManager().ClearTimer(CheckAllKillTimerHandle);

		// 낮으로 전환시키기!
		SetDayNightState(EDayNightState::Day);
	}
}

