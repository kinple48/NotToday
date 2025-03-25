// Fill out your copyright notice in the Description page of Project Settings.


#include "CSW/Item/DropItem.h"
#include "Components/SphereComponent.h"

// Sets default values
ADropItem::ADropItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SetRootComponent(SphereComp);
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(RootComponent);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComp->SetRelativeLocation(MeshLocationOffset);
}

// Called when the game starts or when spawned
void ADropItem::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ADropItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	Rotate(DeltaTime);
}

void ADropItem::Rotate(float DeltaTime)
{
	AddActorLocalRotation(FRotator(0,RotationSpeed * (DeltaTime), 0 ));
}

void ADropItem::Apply(AMainPlayer* Player)
{
	// if (PickUpSound)
	// {
	// 	// 사운드 재생
	// }
	//
	// if (PickUpUIClass)
	// {
	// 	// 3D UI 현위치에서 생성
	// }
	Destroy();
}