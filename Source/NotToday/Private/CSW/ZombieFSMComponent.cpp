// Fill out your copyright notice in the Description page of Project Settings.


#include "CSW/ZombieFSMComponent.h"

#include "AIController.h"
#include "AITypes.h"
#include "Barricade.h"
#include "DefenseTower.h"
#include "MainGameModeBase.h"
#include "MainGameStateBase.h"
#include "MainPlayer.h"
#include "NavigationSystem.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "CSW/Item/ItemDropComponent.h"
#include "CSW/ZombieAnim.h"
#include "CSW/ZombieBase.h"
#include "Kismet/GameplayStatics.h"
#include "Navigation/PathFollowingComponent.h"
#include "NotToday/NotToday.h"

UZombieFSMComponent::UZombieFSMComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UZombieFSMComponent::BeginPlay()
{
	Super::BeginPlay();
	
	auto mainPlayer = UGameplayStatics::GetActorOfClass(GetWorld(), AMainPlayer::StaticClass());
	Player = Cast<AMainPlayer>(mainPlayer);

	HP = MaxHP;
	SetState(EZombieState::Move);

	NS = UNavigationSystemV1::GetNavigationSystem(GetWorld());
	if (!NS)
	{
		PRINT_LOG(CSW, TEXT("No Navigation System"));
		return;
	}
}

void UZombieFSMComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!Me || !Me->Controller)
		return;
	
	if (!AIController)
	{
		AController* Controller = Me->Controller;
		if (Controller)
		{
			AIController = Cast<AAIController>(Controller);
		}
		return;
	}

	ElapsedTime += DeltaTime;
	switch (State)
	{
		case EZombieState::Move: { MoveTick(); } break;
		case EZombieState::Attack: { AttackTick(); } break;
		case EZombieState::Die: { DieTick(); } break;
	}
}

void UZombieFSMComponent::SetState(EZombieState InState)
{
	FString StateName = UEnum::GetValueAsString(InState);
	//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Cyan, StateName);
	State = InState;

	switch (State)
	{
	case EZombieState::Attack:
		AIController->StopMovement();
		break;
	}
}

// void UZombieFSMComponent::IdleTick()
// {
// 	// 타겟 재설정
// 	
// }

void UZombieFSMComponent::MoveTick()
{
	if (Player->HP <= 0)
	{
		// 플레이어가 죽은 상태면 랜덤 이동
		
		return;
	}
	if (!NS)
	{
		NS = UNavigationSystemV1::GetNavigationSystem(GetWorld());
		PRINT_LOG(CSW, TEXT("No Navigation System"));
		return;
	}
	
	FVector PlayerLocation = Player->GetActorLocation();
	DistToPlayer = FVector::Distance(PlayerLocation, Me->GetActorLocation());
	
	//DrawDebugCircle(GetWorld(), Me->GetActorLocation() + FVector::UpVector*2, ChaseRange, 20, FColor::Orange);
	//DrawDebugCircle(GetWorld(), Me->GetActorLocation() + FVector::UpVector*4, AttackRange, 20, FColor::Red);
	
	Target = nullptr;
	FNavLocation NavLocation;
	
	// 1. 타겟을 설정한다
	if (DistToPlayer <= ChaseRange) // 플레이어가 추적가능 거리 이내면,
	{
		FAIMoveRequest Request; // 리퀘스트 생성
		Request.SetAcceptanceRadius(30);
		Request.SetGoalLocation(PlayerLocation);
		
		FPathFindingQuery Query; // 쿼리 생성
		AIController->BuildPathfindingQuery(Request, Query);

		// 길찾기 결과
		FPathFindingResult FindingResult = NS->FindPathSync(Query);
		if (FindingResult.Result == ENavigationQueryResult::Success) // 플레이어 위치까지 길찾기 성공
		{
			Target = Player; // 플레이어를 타겟으로 한다
		}
	}
	
	if (!Target)  // 플레이어 위치까지 길찾기 실패...
	{
		TObjectPtr<ADefenseTower> ClosestTurret = FindClosestTower();
		if (ClosestTurret)
		{
			float DistToTurret = FVector::Distance(Me->GetActorLocation(), ClosestTurret->GetActorLocation());
			if (DistToTurret <= ChaseRange)
			{
				// 터렛으로 갈 수 있는지 확인한다.
				bool bFound = NS->ProjectPointToNavigation(ClosestTurret->GetActorLocation(), NavLocation, SearchExtent/*SearchExtent * 2.f*/);
				if (bFound)
				{
					Target = ClosestTurret;
				}
			}
		}
	}
	
	if (!Target)
	{
		TObjectPtr<ABarricade> ClosestBarricade = FindClosestBarricade();
		if (ClosestBarricade )
		{
			float DistToBarricade = FVector::Distance(Me->GetActorLocation(), ClosestBarricade->GetActorLocation());
			if (DistToBarricade <= ChaseRange)
			{
				// 바리케이드로 갈 수 있는지 확인한다.
				bool bFound = NS->ProjectPointToNavigation(ClosestBarricade->GetActorLocation(), NavLocation, SearchExtent/*SearchExtent * 2.f*/);
				if (bFound)
				{
					Target = ClosestBarricade;
				}
			}
			
		}
	}
	
	if (!Target)
	{
		Target = Player;
		if (!Target)
		{
			PRINT_LOG(CSW, TEXT("No Target Found"));
			return;
		}
	}
	
	// 2. 타겟을 향해 이동한다
	if (Target == Player)
	{
		AIController->MoveToLocation(GetRandomOffsetLocation(Target->GetActorLocation()));
	}
	else
	{
		AIController->MoveToLocation(GetRandomOffsetLocation(NavLocation));
	}


	// 3. 타겟을 공격할 수 있는지 확인한다
	FHitResult Hit;
	FVector Start = Me->GetActorLocation() + Me->GetActorUpVector();
	FVector End = Start + (Target->GetActorLocation() - Me->GetActorLocation()).GetSafeNormal() * AttackRange;
	
	//DrawDebugLine(GetWorld(), Start, End, FColor::Magenta);
	
	bool bHit = GetWorld()->SweepSingleByChannel(Hit, Start, End, FQuat::Identity, ECC_ZombieTarget, FCollisionShape::MakeBox(FVector(0.5f))); 
	if (bHit && !bAttackPlaying)
	{
		Target = Hit.GetActor();
		SetState(EZombieState::Attack);
		ElapsedTime = AttackDelayTime; // 바로 공격 시작
	}
}

void UZombieFSMComponent::AttackTick()
{
	if (!Target)
	{
		SetState(EZombieState::Move);
		return;
	}
	// 타켓을 향해 바라본다
	FVector Dir = (Target->GetActorLocation() - Me->GetActorLocation()).GetSafeNormal();
	Dir.Z = 0.f;
	Me->SetActorRotation(FRotator(0.f, Dir.Rotation().Yaw, 0.f));
	
	if (ElapsedTime >= AttackDelayTime && !bAttackPlaying)
	{
		// 공격!
		ElapsedTime = 0.f;
		Me->PlayAnimMontage(Anim->ZombieMontage, 1.f, TEXT("Attack"));
		AIController->StopMovement();
		bAttackPlaying = true;
	}
	else
	{
		DistToTarget = FVector::Distance(Target->GetActorLocation(), Me->GetActorLocation());
		if (DistToTarget > AttackRange + 10.f && !bAttackPlaying) // 공격범위 벗어남 && 공격중이 아님
		{
			UAnimInstance* AnimInstance = Me->GetMesh()->GetAnimInstance();
			if (AnimInstance)
			{
				if (AnimInstance->Montage_IsPlaying(Anim->ZombieMontage))
				{
					AnimInstance->Montage_Stop(0.f, Anim->ZombieMontage);
				};
			}
			
			SetState(EZombieState::Move);
		}
	}
}

void UZombieFSMComponent::DieTick()
{
	// 일정시간 후에
	if (ElapsedTime >= DieDestoryTime)
	{
		// 좀비가 죽음을 알린다.
		auto MainGameState = Cast<AMainGameStateBase>(GetWorld()->GetGameState());
		MainGameState->ZombieKilled(Me);
		PRINT_LOG(CSW, TEXT("DieTick"));
		// 제거한다	
		Me->Destroy();
	}
}


void UZombieFSMComponent::OnDamage(int32 damage)
{
	HP -= damage;

	if (HP > 0)
	{
		// 피격
		Me->PlayAnimMontage(Anim->ZombieMontage, 3.0f, TEXT("Damage"));
	}
	else
	{
		// 사망
		SetState(EZombieState::Die);
		ElapsedTime = 0.f;
		Me->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Me->GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Me->PlayAnimMontage(Anim->ZombieMontage, 1.3f, TEXT("Die"));
		AIController->StopMovement();

		Me->ItemDrop->DropItem();

		// 돈 획득
		Player->CashData += 100.f;
		if (auto GameMode = GetWorld()->GetAuthGameMode())
		{
			if (auto MainGameMode = Cast<AMainGameModeBase>(GameMode))
			{
				MainGameMode->PrintCash();
			}
		}
	}
}


TObjectPtr<ABarricade> UZombieFSMComponent::FindClosestBarricade()
{
	AActor* Closest { nullptr };
	
	TArray<AActor*> Barricades;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABarricade::StaticClass(), Barricades);

	float MinDistance = FLT_MAX; // float 최대값
	for (AActor* Barricade : Barricades)
	{
		FVector v1 = Me->GetActorLocation();
		FVector v2 = Barricade->GetActorLocation();
		
		float dist = FMath::Sqrt(FMath::Square(v1.X-v2.X) + FMath::Square(v1.Y-v2.Y));
		if (dist < MinDistance)
		{
			MinDistance = dist;
			Closest = Barricade;
		}
	}
	return Cast<ABarricade>(Closest);
}

TObjectPtr<ADefenseTower> UZombieFSMComponent::FindClosestTower()
{
	AActor* Closest { nullptr };
	
	TArray<AActor*> Turrets;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADefenseTower::StaticClass(), Turrets);

	float MinDistance = FLT_MAX; // float 최대값
	for (AActor* Turret : Turrets)
	{
		FVector v1 = Me->GetActorLocation();
		FVector v2 = Turret->GetActorLocation();
		
		float dist = FMath::Sqrt(FMath::Square(v1.X-v2.X) + FMath::Square(v1.Y-v2.Y));
		if (dist < MinDistance)
		{
			MinDistance = dist;
			Closest = Turret;
		}
	}
	return Cast<ADefenseTower>(Closest);
}

FVector UZombieFSMComponent::GetRandomOffsetLocation(FVector Location)
{
	float RandomX = FMath::RandRange(-SpreadRadius, SpreadRadius);
	float RandomY = FMath::RandRange(-SpreadRadius, SpreadRadius);
	return Location + FVector(RandomX, RandomY, 0.0f);
}
