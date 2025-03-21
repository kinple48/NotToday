// Fill out your copyright notice in the Description page of Project Settings.


#include "CSW/ZombieBase.h"

AZombieBase::AZombieBase()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AZombieBase::BeginPlay()
{
	Super::BeginPlay();
	
}


void AZombieBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AZombieBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AZombieBase::SetTarget(AActor* InTarget)
{
	Target = InTarget;
}

AActor* AZombieBase::GetTarget() const
{
	return Target;
}
