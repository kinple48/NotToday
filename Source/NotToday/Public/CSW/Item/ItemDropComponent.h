// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ItemDropComponent.generated.h"

class AZombieBase;
class ADropItem;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NOTTODAY_API UItemDropComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UItemDropComponent();

public:

	// 컴포넌트 소유 액터
	UPROPERTY(EditAnywhere)
	TObjectPtr<AZombieBase> Me;	
	
	UPROPERTY(EditAnywhere, Category = ItemDrop)
	TArray<TSubclassOf<ADropItem>> ItemDropClasses;

	void DropItem();

	UPROPERTY(EditAnywhere, Category = ItemDrop)
	float RandToDrop {0.5f};
};
