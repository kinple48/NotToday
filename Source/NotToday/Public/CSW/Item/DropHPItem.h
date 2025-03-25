// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DropItem.h"
#include "DropHPItem.generated.h"

class AMainPlayer;
/**
 * 
 */
UCLASS()
class NOTTODAY_API ADropHPItem : public ADropItem
{
	GENERATED_BODY()
public:
	ADropHPItem();
	virtual void Apply(AMainPlayer* Player) override;
};

