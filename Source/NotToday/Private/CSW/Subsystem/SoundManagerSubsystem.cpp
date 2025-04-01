// Fill out your copyright notice in the Description page of Project Settings.


#include "CSW/Subsystem/SoundManagerSubsystem.h"

#include "Kismet/GameplayStatics.h"
#include "NotToday/NotToday.h"
#include "Sound/SoundCue.h"

TMap<ESoundType, FString> USoundManagerSubsystem::SoundMap = {
	{ESoundType::AmmoBoxPickUp, "/Script/Engine.SoundWave'/Game/SFX/AmmoBoxPickUp_Cue.AmmoBoxPickUp_Cue'"},
	{ESoundType::HealthPickUp, "/Script/Engine.SoundWave'/Game/SFX/HealthPickUp_Cue.HealthPickUp_Cue'"},
	{ESoundType::MoneyPickUp, "/Script/Engine.SoundWave'/Game/SFX/MoneyPickUp_Cue.MoneyPickUp_Cue'"},
	{ESoundType::NightStart, "/Script/Engine.SoundWave'/Game/SFX/NightStart_Cue.NightStart_Cue'"},
	{ESoundType::Reload, "/Script/Engine.SoundWave'/Game/SFX/Reload_Cue.Reload_Cue'"},
	{ESoundType::ZombieAttack, "/Script/Engine.SoundWave'/Game/SFX/ZombieAttack_Cue.ZombieAttack_Cue'"},
	{ESoundType::ZombieDie, "/Script/Engine.SoundWave'/Game/SFX/ZombieDie_Cue.ZombieDie_Cue'"},
	{ESoundType::ZombieHit, "/Script/Engine.SoundCue'/Game/SFX/ZombieHit_Cue.ZombieHit_Cue'"}
};

void USoundManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
}

void USoundManagerSubsystem::Deinitialize()
{
	Super::Deinitialize();
	CachedSoundMap.Empty();
}

USoundCue* USoundManagerSubsystem::FindSoundCue(ESoundType SoundType)
{
	if (!SoundMap.Contains(SoundType))
		return nullptr;

	// 캐시되어 있는지 확인
	if (CachedSoundMap.Contains(SoundType))
	{
		return CachedSoundMap[SoundType];
	}

	FString Path = SoundMap[SoundType];
	USoundCue* SCue = LoadObject<USoundCue>(nullptr, *Path);

	CachedSoundMap.Add(SoundType, SCue);
	return SCue;
}

void USoundManagerSubsystem::PlaySound2D(USoundCue* Cue)
{       
	UGameplayStatics::PlaySound2D(GetWorld(), Cue);
}

void USoundManagerSubsystem::PlaySound2D(ESoundType SoundType)
{
	auto cue = FindSoundCue(SoundType);
	if (cue)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), FindSoundCue(SoundType));
	}
	else
	{
		PRINT_LOG(CSW, TEXT("FindSoundCue(SoundType) == nullptr..."));
	}
}

void USoundManagerSubsystem::PlaySoundAtLocation(USoundCue* Cue, FVector Location)
{
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), Cue, Location);
}

void USoundManagerSubsystem::PlaySoundAtLocation(ESoundType SoundType, FVector Location)
{
	auto cue = FindSoundCue(SoundType);
	if (cue)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), FindSoundCue(SoundType), Location);
	}
	else
	{
		PRINT_LOG(CSW, TEXT("FindSoundCue(SoundType) == nullptr..."));
	}
}
