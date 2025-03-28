// Fill out your copyright notice in the Description page of Project Settings.


#include "Barricade.h"
#include "Components/BoxComponent.h"

// Sets default values
ABarricade::ABarricade()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	boxcomp = CreateDefaultSubobject<UBoxComponent>( TEXT( "boxcomp" ) );
	boxcomp->SetupAttachment( RootComponent );

	meshcomp = CreateDefaultSubobject<UStaticMeshComponent>( TEXT( "meshcomp" ) );
	meshcomp->SetupAttachment( boxcomp );
	meshcomp->SetCollisionEnabled( ECollisionEnabled::NoCollision );

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
		this->Destroy();
	}
}

