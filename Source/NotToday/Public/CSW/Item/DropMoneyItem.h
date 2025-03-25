// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DropItem.h"
#include "DropMoneyItem.generated.h"

/**
 * 
 */
UCLASS()
class NOTTODAY_API ADropMoneyItem : public ADropItem
{
	GENERATED_BODY()
public:
	ADropMoneyItem();
	virtual void Apply(AMainPlayer* Player) override;
};
