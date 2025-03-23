// Fill out your copyright notice in the Description page of Project Settings.


#include "CSW/ZombieBase.h"
#include "GameFramework/Actor.h"
#include "NotToday/NotToday.h"
#include "CSW/EnemyTableRow.h"
#include "GameFramework/CharacterMovementComponent.h"

AZombieBase::AZombieBase()
{
	PrimaryActorTick.bCanEverTick = true;

	// 충돌 세팅
	
	
	// 캐릭터 무브먼트
	GetCharacterMovement()->bOrientRotationToMovement = true;
	
}

void AZombieBase::BeginPlay()
{
	Super::BeginPlay();
	HP = MaxHP;
}

void AZombieBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	SetData(DTRowHandle);
}

void AZombieBase::SetData(const FDataTableRowHandle& InDataTableRowHandle)
{
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

	//PRINT_LOG(CSW, TEXT("SetData) %s - HP: %d, Damage: %d"), *ClassName, 		
	GetCharacterMovement()->MaxWalkSpeed = Data->MaxWalkSpeed;
	GetMesh()->SetSkeletalMesh(Data->SkeletalMesh);
	GetMesh()->SetRelativeTransform(Data->MeshOffsetTransform);
	GetMesh()->AnimClass = Data->Anim;

	MaxHP = Data->MaxHP;
	Damage = Data->Damage;
	AttackDelayTime = Data->AttackDelayTime;

	AttackMontage = Data->AttackMontage;
	DieMontage = Data->DieMontage;
	
}

void AZombieBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AZombieBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AZombieBase::SetAttackTarget(AActor* InTarget)
{
	AttackTarget = InTarget;
}



AActor* AZombieBase::GetTarget() const
{
	return AttackTarget;
}

