// Fill out your copyright notice in the Description page of Project Settings.


#include "CSW/Item/ItemDropComponent.h"

#include "CSW/ZombieBase.h"
#include "CSW/Item/DropItem.h"

UItemDropComponent::UItemDropComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UItemDropComponent::DropItem()
{
	if (ItemDropClasses.Num() <= 0)
		return;

	float r = FMath::FRandRange(0.f, 1.f);
	if (r >= RandToDrop)
		return;
	
	int randNum = FMath::RandRange(0, ItemDropClasses.Num()-1);
	GetWorld()->SpawnActor<ADropItem>(ItemDropClasses[randNum], Me->GetActorLocation(), FRotator(0, 0, 0));
}

