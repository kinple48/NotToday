// Fill out your copyright notice in the Description page of Project Settings.


#include "MainGameStateBase.h"

#include "EngineUtils.h"
#include "CSW/WaveTableRow.h"
#include "CSW/ZombieSpawner.h"
#include "NotToday/NotToday.h"
#include "MainPlayer.h"
#include "CSW/Item/DropItem.h"
#include "CSW/Subsystem/SoundManagerSubsystem.h"
#include "Kismet/GameplayStatics.h"

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
	
	OnNightStarted.AddDynamic(this, &AMainGameStateBase::PlayWaveStartSound);

	OnZombieKilled.AddDynamic(this, &AMainGameStateBase::DecreaseAndCheckLeftToKill); // DecreaseAndCheckLeftToKill
	
	LoadNextWaveData();
	ZombieSpawner->SpawnZombie();
}

void AMainGameStateBase::SetDayNightState(EDayNightState NewState)
{
	State = NewState;
	
	if (State == EDayNightState::Day)
	{
		PRINT_LOG(CSW, TEXT("SetDayNightState --> Day"));
		OnDayStarted.Broadcast();
	}
	else
	{
		PRINT_LOG(CSW, TEXT("SetDayNightState --> Night"));
		
		OnNightStarted.Broadcast();
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

void AMainGameStateBase::PlayWaveStartSound()
{
	USoundManagerSubsystem* SoundManager = GetGameInstance()->GetSubsystem<USoundManagerSubsystem>();
	if (SoundManager)
	{
		SoundManager->PlaySound2D(ESoundType::NightStart);
	}
}

void AMainGameStateBase::ItemMagnet()
{
	ItemElapsedTime += 0.03f;
    float Alpha = FMath::Clamp(ItemElapsedTime / ItemMagnetDuration, 0.0f, 1.0f);
	bool bItemExist = false;
	// 모든 아이템들이 플레이어를 향해 날아오도록 한다.
	for (TActorIterator<ADropItem> It(GetWorld()); It; ++It)
	{
		FVector NextLocation = FMath::Lerp(It->GetActorLocation(), player->GetActorLocation(), Alpha);
		It->SetActorLocation(NextLocation);
		bItemExist = true;
	}

	if (!bItemExist)
	{
		GetWorld()->GetTimerManager().ClearTimer(ItemMagnetTimerHandle);
	}
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
		ItemElapsedTime = 0.f;
		
		// 모든 아이템을 플레이어가 먹도록 유도하기
		GetWorld()->GetTimerManager().SetTimer(ItemMagnetTimerHandle, this, &AMainGameStateBase::ItemMagnet,
			0.03f, true);

		// 낮으로 전환시키기!
		SetDayNightState(EDayNightState::Day);
	}
}


