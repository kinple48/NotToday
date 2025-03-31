// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SoundManagerSubsystem.generated.h"

UENUM(BlueprintType)
enum class ESoundType : uint8
{
	AmmoBoxPickUp,
	MoneyPickUp,
	NightStart,
	Reload,
	ZombieAttack,
	ZombieDie,
	ZombieHit,
};
/**
 * 
 */
UCLASS()
class NOTTODAY_API USoundManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Deinitialize() override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	USoundCue* FindSoundCue(ESoundType SoundType);
	
	void PlaySound2D(USoundCue* Cue);
	void PlaySound2D(ESoundType SoundType);
	void PlaySoundAtLocation(USoundCue* Cue, FVector Location);
	void PlaySoundAtLocation(ESoundType SoundType, FVector Location);

private:
	static TMap<ESoundType, FString> SoundMap;
	
	TMap<ESoundType, USoundCue*> CachedSoundMap;
};
