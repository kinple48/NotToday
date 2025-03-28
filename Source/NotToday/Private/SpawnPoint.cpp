// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnPoint.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "MainPlayer.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ASpawnPoint::ASpawnPoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	boxcomp = CreateDefaultSubobject<UBoxComponent>(TEXT("boxcomp"));
	boxcomp->SetupAttachment(RootComponent);

	meshcomp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("meshcomp"));
	meshcomp->SetupAttachment(boxcomp);
	meshcomp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	meshcomp->SetVisibility(false);

	// StaticMesh 설정
	ConstructorHelpers::FObjectFinder<UStaticMesh>Temp_Object(TEXT("/Script/Engine.StaticMesh'/Game/LJW/Asset/Barricade/source/SM_Barricade.SM_Barricade'"));
	if (Temp_Object.Succeeded())
	{
		meshcomp->SetStaticMesh(Temp_Object.Object);
	}
	/*ConstructorHelpers::FObjectFinder <UMaterial> Temp_Material( TEXT( "/Script/Engine.Material'/Game/LJW/Asset/Barricade/source/tmp_Barricade.tmp_Barricade'" ) );
	if (Temp_Material.Succeeded())
	{
		meshcomp->SetMaterial( 0,Temp_Material.Object );
	}*/
}

// Called when the game starts or when spawned
void ASpawnPoint::BeginPlay()
{
	Super::BeginPlay();
	AActor* actor = UGameplayStatics::GetActorOfClass(GetWorld(), AMainPlayer::StaticClass());
	if (actor)
	{
		player = Cast<AMainPlayer>(actor);
	}
}

// Called every frame
void ASpawnPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}