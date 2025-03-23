// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyInterface.h"
#include "GameFramework/Character.h"
#include "ZombieBase.generated.h"

struct FEnemyTableRow;
class UAnimInstance;
class UAnimInstance;

UCLASS(Abstract)
class NOTTODAY_API AZombieBase : public ACharacter, public IEnemyInterface
{
	GENERATED_BODY()

public:
	AZombieBase();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void SetAttackTarget(AActor* InTarget) override;
	
protected:
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;
	
	UPROPERTY(EditAnywhere, meta = (RowType="/Script/NotToday.EnemyTableRow")) // /Script/모듈명
	FDataTableRowHandle DTRowHandle;
	FEnemyTableRow* Data;

	UPROPERTY(EditAnywhere)
	AActor* AttackTarget; // 공격대상
	
public:
	// IEnemyInterface
	void SetData(const FDataTableRowHandle& InDataTableRowHandle	);
	FORCEINLINE FEnemyTableRow* GetData() const { return Data; }
	
	virtual AActor* GetTarget() const override;
	
private:
	int32 MaxHP;
	int32 HP;
	int32 Damage; // 공격력
	float AttackDelayTime;
	float AttackRange; // 사거리
	UPROPERTY(EditDefaultsOnly, Category=Anim)
	TSubclassOf<UAnimInstance> Anim;
	UPROPERTY(EditDefaultsOnly, Category=Anim)
	TObjectPtr<UAnimMontage> AttackMontage;
	UPROPERTY(EditDefaultsOnly, Category=Anim)
	TObjectPtr<UAnimMontage> DieMontage;
};
