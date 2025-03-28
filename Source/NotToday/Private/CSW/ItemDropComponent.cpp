// Fill out your copyright notice in the Description page of Project Settings.


#include "CSW/ItemDropComponent.h"

#include "CSW/DropItem.h"
#include "CSW/ZombieBase.h"
#include "Kismet/GameplayStatics.h"

UItemDropComponent::UItemDropComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UItemDropComponent::DropItem()
{
	if (ItemDropClasses.Num() <= 0)
		return;
	
	int randNum = FMath::RandRange(0, ItemDropClasses.Num()-1);
	GetWorld()->SpawnActor<ADropItem>(ItemDropClasses[randNum], Me->GetActorLocation(), FRotator(0, 0, 0));
}

