// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyInterface.h"
#include "Components/ActorComponent.h"
#include "ZombieFSMComponent.generated.h"

class ABarricade;
class AMainPlayer;
class AZombieBase;

UENUM(BlueprintType)
enum class EZombieState : uint8
{
	//Idle,
	Move,
	Attack,
	//Damage, //피격
	Die,
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NOTTODAY_API UZombieFSMComponent : public UActorComponent, public IEnemyInterface
{
	GENERATED_BODY()

public:	
	UZombieFSMComponent();
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	virtual void BeginPlay() override;

public:
	// 컴포넌트 소유 액터
	UPROPERTY()
	TObjectPtr<AZombieBase> Me;
	
	UPROPERTY()
	class UZombieAnim* Anim;

	// AI
	UPROPERTY()
	class AAIController* AIController;

	// 공격대상
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<AMainPlayer> Player;
	
	UPROPERTY(VisibleAnywhere)
	float DistToPlayer;
	
	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<ABarricade>> Barricades;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<AActor> Target;
	
	UPROPERTY(VisibleAnywhere)
	float DistToTarget;
	
	virtual void SetTarget(TObjectPtr<AActor> InTarget) override;
	virtual TObjectPtr<AActor> GetTarget() const override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = FSM)
	EZombieState State;

	TObjectPtr<ABarricade> FindClosestBarricade();
	
	//void IdleTick();
	void MoveTick();
	void AttackTick();
	//void DamageTick();
	void DieTick();

	void SetState(EZombieState InState);

	// 현재 상태에서 경과시간
	UPROPERTY(VisibleAnywhere)
	float ElapsedTime { 0.f };
	
	// 체력
	UPROPERTY(EditAnywhere, Category = FSM)
	int32 MaxHP;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FSM)
	int32 HP { MaxHP };

	// 쫓기 거리
	UPROPERTY(EditAnywhere, Category = FSM)
	float ChaseRange = 500.0f; // 5m
	
	// 공격 
	UPROPERTY(EditAnywhere, Category = FSM)
	float AttackRange; // 50cm

	UPROPERTY(EditAnywhere, Category = FSM)
	float AttackDelayTime;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = FSM)
	bool bAttackPlaying = false;

	UPROPERTY(EditAnywhere, Category = FSM)
	int32 Damage; // 공격력
	
	// 피격
	void OnDamage(int32 damage);
	


	// 사망
	UPROPERTY(EditAnywhere, Category = FSM)
	float DieDestoryTime {5.f};
	//bool bDieDone { false };
	
	//FORCEINLINE void OnDieEnd() { bDieDone = true; }
};
