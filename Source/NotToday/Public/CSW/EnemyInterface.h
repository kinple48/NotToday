// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "EnemyInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UEnemyInterface : public UInterface
{
	GENERATED_BODY()
};

class NOTTODAY_API IEnemyInterface
{
	GENERATED_BODY()

public:
	// virtual void SetTarget(TObjectPtr<AActor> InTarget) = 0;
	// virtual TObjectPtr<AActor> GetTarget() const = 0;
};
