// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ZombieBase.generated.h"

class UBoxComponent;
class USphereComponent;
class UNavigationInvokerComponent;
class UZombieAnim;
class UItemDropComponent;
class UZombieFSMComponent;
struct FEnemyTableRow;
class UAnimInstance;
class UAnimInstance;

UCLASS(Abstract)
class NOTTODAY_API AZombieBase : public ACharacter
{
	GENERATED_BODY()

public:
	
	AZombieBase();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void PostInitializeComponents() override; // 모든 컴포넌트가 초기화된 후 호출
	
	UPROPERTY(EditAnywhere, meta = (RowType="/Script/NotToday.EnemyTableRow"), Category = Data) // /Script/모듈명
	FDataTableRowHandle DTRowHandle;
	FEnemyTableRow* Data;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = FSM)
	TObjectPtr<UZombieFSMComponent> FSM;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AI)
	TObjectPtr<UNavigationInvokerComponent> NavInvoker;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ItemDrop)
	TObjectPtr<UItemDropComponent> ItemDrop;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ItemDrop)
	TObjectPtr<UBoxComponent> MeleeHitBox;
	
	void SetData(const FDataTableRowHandle& InDataTableRowHandle);
	FORCEINLINE FEnemyTableRow* GetData() const { return Data; }

	// 근접공격 히트박스 충돌함수
	UFUNCTION()
	void OnMeleeHitBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
								   const FHitResult& SweepResult);
};
