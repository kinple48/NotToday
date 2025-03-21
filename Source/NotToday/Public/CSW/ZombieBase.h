// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyInterface.h"
#include "GameFramework/Character.h"
#include "ZombieBase.generated.h"

struct FEnemyTableRow;

UCLASS(Abstract)
class NOTTODAY_API AZombieBase : public ACharacter, public IEnemyInterface
{
	GENERATED_BODY()

public:
	AZombieBase();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void SetTarget(AActor* InTarget) override;
	
protected:
	virtual void BeginPlay() override;

	FEnemyTableRow* Data; 

public:
	FEnemyTableRow* SetData(const FDataTableRowHandle& InDataTableRowHandle	);
	FORCEINLINE FEnemyTableRow* GetData() const { return Data; }
	virtual AActor* GetTarget() const override;
private:
	AActor* Target;
};
