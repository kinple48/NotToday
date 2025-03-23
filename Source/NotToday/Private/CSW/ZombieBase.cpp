// Fill out your copyright notice in the Description page of Project Settings.


#include "CSW/ZombieBase.h"

#include "AIController.h"
#include "MainPlayer.h"
#include "NavigationInvokerComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Actor.h"
#include "CSW/EnemyTableRow.h"
#include "CSW/ItemDropComponent.h"
#include "CSW/ZombieAnim.h"
#include "CSW/ZombieFSMComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "NotToday/NotToday.h"

AZombieBase::AZombieBase()
{
	PrimaryActorTick.bCanEverTick = true;
	PRINT_LOG(CSW, TEXT("AZombieBase::AZombieBase()"));

	// 충돌 세팅
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block); //플레이어 공격과 상호작용 O
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	
	// 캐릭터 무브먼트
	GetCharacterMovement()->bOrientRotationToMovement = true;

	// 유한상태머신
	FSM = CreateDefaultSubobject<UZombieFSMComponent>(TEXT("FSM"));

	// 네비게이션 인보커
	NavInvoker = CreateDefaultSubobject<UNavigationInvokerComponent>(TEXT("NavInvoker"));
	NavInvoker->SetGenerationRadii(1000.f, 1200.f);

	// 아이템 드랍
	ItemDropComponent = CreateDefaultSubobject<UItemDropComponent>(TEXT("ItemDropComponent"));
	
}

void AZombieBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	PRINT_LOG(CSW, TEXT("AZombieBase::OnConstruction"));
	SetData(DTRowHandle);
}

void AZombieBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	PRINT_LOG(CSW, TEXT("AZombieBase::PostInitializeComponents"));
	if (!Data)
	{
		SetData(DTRowHandle);
	}
	
	GetCharacterMovement()->MaxWalkSpeed = Data->MaxWalkSpeed;
	
	GetMesh()->SetSkeletalMesh(Data->SkeletalMesh);
	GetMesh()->SetRelativeTransform(Data->MeshOffsetTransform);
	GetMesh()->SetAnimInstanceClass(Data->AnimBP->GeneratedClass);

	// FSM 컴포넌트 초기화
	FSM->Me = this;
	FSM->Anim = Cast<UZombieAnim>(GetMesh()->GetAnimInstance());
	FSM->Anim->ZombieMontage = Data->Montage;
	FSM->AIController = Cast<AAIController>(GetController());
	
	FSM->MaxHP = Data->MaxHP;
	FSM->Damage = Data->Damage;
	FSM->AttackDelayTime = Data->AttackDelayTime;
	FSM->AttackRange = Data->AttackRange;

	// ItemDrop 컴포넌트 초기화
}

void AZombieBase::SetData(const FDataTableRowHandle& InDataTableRowHandle)
{
	PRINT_LOG(CSW, TEXT("AZombieBase::SetData"));

	if (InDataTableRowHandle.IsNull()) { return; }
	DTRowHandle = InDataTableRowHandle;
	if (!DTRowHandle.DataTable)
	{
		UE_LOG(LogTemp, Error, TEXT("DataTable is NULL!"));
		return;
	}
	
	FString ClassName = GetClass()->GetName();
	ClassName.RemoveFromStart(TEXT("BP_"));
	ClassName.RemoveFromEnd(TEXT("_C"));
	UE_LOG(LogTemp, Warning, TEXT("SetData) %s"), *ClassName);
	
	FEnemyTableRow* DataRow = DTRowHandle.GetRow<FEnemyTableRow>(DTRowHandle.RowName.ToString());
	if (!DataRow)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to get DataRow for %s"), *ClassName);
		return;
	}
	
	Data = DataRow;
	UE_LOG(LogTemp, Warning, TEXT("HP: %d, Damage: %d"), Data->MaxHP, Data->Damage);
}

void AZombieBase::BeginPlay()
{
	Super::BeginPlay();
	PRINT_LOG(CSW, TEXT("AZombieBase::BeginPlay"));

}

void AZombieBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
