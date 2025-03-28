// Fill out your copyright notice in the Description page of Project Settings.


#include "CSW/ZombieNavVolume.h"

#include "NavigationSystem.h"

AZombieNavVolume::AZombieNavVolume()
{
	SetActorScale3D(FVector(20.f, 20.f, 1.f));
}

void AZombieNavVolume::BeginPlay()
{
	Super::BeginPlay();

	//SetupNavVolume();
}

void AZombieNavVolume::SetupNavVolume()
{
	UNavigationSystemV1* ns = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	if (ns)
	{
		ns->OnNavigationBoundsUpdated(this);

		ns->Build();
	}
}

