// Fill out your copyright notice in the Description page of Project Settings.


#include "CSW/Subsystem/SoundManagerSubsystem.h"

#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

TMap<ESoundType, FString> USoundManagerSubsystem::SoundMap = {
	{ESoundType::AmmoBoxPickUp, "/Script/Engine.SoundWave'/Game/SFX/AmmoBoxPickUp.AmmoBoxPickUp'"},
	{ESoundType::MoneyPickUp, "/Script/Engine.SoundWave'/Game/SFX/MoneyPickUp.MoneyPickUp'"},
	{ESoundType::NightStart, "/Script/Engine.SoundWave'/Game/SFX/NightStart.NightStart'"},
	{ESoundType::Reload, "/Script/Engine.SoundWave'/Game/SFX/Reload.Reload'"},
	{ESoundType::ZombieAttack, "/Script/Engine.SoundWave'/Game/SFX/ZombieAttack.ZombieAttack'"},
	{ESoundType::ZombieDie, "/Script/Engine.SoundWave'/Game/SFX/ZombieDie.ZombieDie'"},
	{ESoundType::ZombieHit, "/Script/Engine.SoundWave'/Game/SFX/ZombieHit.ZombieHit'"}
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
	UGameplayStatics::PlaySound2D(GetWorld(), FindSoundCue(SoundType));
}

void USoundManagerSubsystem::PlaySoundAtLocation(USoundCue* Cue, FVector Location)
{
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), Cue, Location);
}

void USoundManagerSubsystem::PlaySoundAtLocation(ESoundType SoundType, FVector Location)
{
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), FindSoundCue(SoundType), Location);
}
