// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NavMesh/NavMeshBoundsVolume.h"
#include "ZombieNavVolume.generated.h"

/**
 * 
 */
UCLASS()
class NOTTODAY_API AZombieNavVolume : public ANavMeshBoundsVolume
{
	GENERATED_BODY()
public:
	AZombieNavVolume();

protected:
	virtual void BeginPlay() override;

public:
	void UpdateNavVolume();	
};
