// Fill out your copyright notice in the Description page of Project Settings.


#include "Barricade.h"

#include "EngineUtils.h"
#include "NavModifierComponent.h"
#include "Components/BoxComponent.h"
#include "CSW/ZombieNavVolume.h"
#include "NavAreas/NavArea_Obstacle.h"
#include "NotToday/NotToday.h"

class AZombieNavVolume;
// Sets default values
ABarricade::ABarricade()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	boxcomp = CreateDefaultSubobject<UBoxComponent>( TEXT( "boxcomp" ) );
	boxcomp->SetupAttachment( RootComponent );
	boxcomp->SetCollisionResponseToChannel(ECC_ZombieTarget, ECR_Block); // ZombieTarget 채널
	boxcomp->SetCollisionResponseToChannel(ECC_ZombieHitBox, ECR_Overlap); // Zombie의 공격 히트박스 

	meshcomp = CreateDefaultSubobject<UStaticMeshComponent>( TEXT( "meshcomp" ) );
	meshcomp->SetupAttachment( boxcomp );
	meshcomp->SetCollisionEnabled( ECollisionEnabled::NoCollision );
	meshcomp->SetCollisionResponseToChannel(ECC_ZombieTarget, ECR_Block); // ZombieTarget 채널
	meshcomp->SetCollisionResponseToChannel(ECC_ZombieHitBox, ECR_Overlap); // Zombie의 공격 히트박스 

	navModComp = CreateDefaultSubobject<UNavModifierComponent>( TEXT( "navModComp" ) );
	navModComp->SetAreaClass(UNavArea_Obstacle::StaticClass());
	navModComp->FailsafeExtent = FVector(14.f, 30.f, 100.f);

	ConstructorHelpers::FObjectFinder<UStaticMesh>Temp_Object(TEXT("/Script/Engine.StaticMesh'/Game/LJW/Asset/Barricade/source/SM_Barricade.SM_Barricade'"));
	if (Temp_Object.Succeeded())
	{
		meshcomp->SetStaticMesh(Temp_Object.Object);
	}
	ConstructorHelpers::FObjectFinder <UMaterial> Temp_Material( TEXT( "/Script/Engine.Material'/Game/LJW/Asset/Barricade/source/Barricade.Barricade'" ) );
	if (Temp_Material.Succeeded())
	{
		meshcomp->SetMaterial( 0,Temp_Material.Object );
	}

}

// Called when the game starts or when spawned
void ABarricade::BeginPlay()
{
	Super::BeginPlay();
	HP = HPMax;

	for (TActorIterator<AZombieNavVolume> It(GetWorld()); It; ++It)
	{
		It->UpdateNavVolume(); // 첫 번째 AZombieNavVolume을 반환
	}
}

// Called every frame
void ABarricade::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABarricade::SetDamage( int32 damage )
{
	HP -= damage;

	if (HP <= 0)
	{
		for (TActorIterator<AZombieNavVolume> It(GetWorld()); It; ++It)
		{
			It->UpdateNavVolume(); // 첫 번째 AZombieNavVolume을 반환
		}
		this->Destroy();
	}
}

