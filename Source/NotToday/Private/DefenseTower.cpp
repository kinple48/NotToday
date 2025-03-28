// Fill out your copyright notice in the Description page of Project Settings.


#include "DefenseTower.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "CSW/ZombieBase.h"
#include "CSW/ZombieFSMComponent.h"
#include "UObject/FastReferenceCollector.h"

// Sets default values
ADefenseTower::ADefenseTower()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	boxcomp = CreateDefaultSubobject<UBoxComponent>( TEXT( "boxcomp" ) );
	boxcomp->SetupAttachment( RootComponent );

	meshcomp = CreateDefaultSubobject<UStaticMeshComponent>( TEXT( "meshcomp" ) );
	meshcomp->SetupAttachment( boxcomp );
	meshcomp->SetCollisionEnabled( ECollisionEnabled::NoCollision );

	spherecomp = CreateDefaultSubobject<USphereComponent>( TEXT( "spherecomp" ) );
	spherecomp->SetupAttachment( boxcomp );
	spherecomp->SetSphereRadius( 500 );

	ConstructorHelpers::FObjectFinder<UStaticMesh>Temp_Object( TEXT( "/Script/Engine.StaticMesh'/Game/LJW/Asset/defence-tower/source/StaticMesh.StaticMesh'" ) );
	if (Temp_Object.Succeeded())
	{
		meshcomp->SetStaticMesh( Temp_Object.Object );
	}
	ConstructorHelpers::FObjectFinder <UMaterial> Temp_Material( TEXT( "/Script/Engine.Material'/Game/LJW/Asset/defence-tower/source/Textrue9.Textrue9'" ) );
	if (Temp_Material.Succeeded())
	{
		meshcomp->SetMaterial( 0 , Temp_Material.Object );
	}
}

// Called when the game starts or when spawned
void ADefenseTower::BeginPlay()
{
	Super::BeginPlay();
	spherecomp->OnComponentBeginOverlap.AddDynamic(this, &ADefenseTower::OnSphereBeginOverlap);
	spherecomp->OnComponentEndOverlap.AddDynamic( this , &ADefenseTower::OnSphereEndOverlap );
}

// Called every frame
void ADefenseTower::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	AActor* ClosestEnemy = nullptr;
	float MinAngle = FLT_MAX;

	for (AActor* Enemy : DetectedEnemies)
	{
		if (!Enemy) continue;

		
		FVector TowerForward = GetActorForwardVector();
		FVector DirectionToEnemy = (Enemy->GetActorLocation() - GetActorLocation()).GetSafeNormal();

		float DotProduct = FVector::DotProduct( TowerForward , DirectionToEnemy );
		float Angle = FMath::Acos( DotProduct );

		if (Angle < MinAngle)
		{
			MinAngle = Angle;
			ClosestEnemy = Enemy;
		}
	}

	if (ClosestEnemy)
	{
		FVector Start = GetActorLocation();
		FVector End = ClosestEnemy->GetActorLocation();

		// 방향 벡터를 기반으로 회전값 계산
		FRotator LookAtRotation = FRotationMatrix::MakeFromX( End - Start ).Rotator();

		// Z축(Yaw)만 사용하고 Pitch와 Roll은 고정
		FRotator CurrentRotation = meshcomp->GetComponentRotation();
		FRotator NewRotation = FRotator( CurrentRotation.Pitch , LookAtRotation.Yaw , CurrentRotation.Roll );

		// 메쉬 회전 적용
		meshcomp->SetWorldRotation( NewRotation );
	}

	CurrentTime += DeltaTime;
	auto enemy = Cast<AZombieBase>( ClosestEnemy );
	if (enemy && CurrentTime >= AttackDelayTime)
	{
		FHitResult HitResult;

		FVector Start = GetActorLocation(); 
		FVector End = enemy->GetActorLocation(); 

		FCollisionQueryParams CollisionParams;
		CollisionParams.AddIgnoredActor( this ); 

		bool bHit = GetWorld()->LineTraceSingleByChannel( HitResult , Start , End , ECC_Visibility , CollisionParams );

		if (bHit)
		{
			GEngine->AddOnScreenDebugMessage( -1 , 0.5f , FColor::Red , TEXT( "Hit" ) );
			enemy->FSM->OnDamage( 2 );
		}

		
		DrawDebugLine( GetWorld() , Start , End , bHit ? FColor::Blue : FColor::Red , false , 0.1f , 0 , 2.0f );
		CurrentTime = 0.0f;
	}
}

void ADefenseTower::OnSphereBeginOverlap( UPrimitiveComponent* OverlappedComponent , AActor* OtherActor , UPrimitiveComponent* OtherComp , int32 OtherBodyIndex , bool bFromSweep , const FHitResult& SweepResult )
{
	auto enemy = Cast<AZombieBase>( OtherActor );
	if (enemy)
	{
		GEngine->AddOnScreenDebugMessage( -1 , 0.5f , FColor::Red , TEXT( "Zombie detected" ) );

		if (!DetectedEnemies.Contains( enemy ))
		{
			DetectedEnemies.Add( enemy );
			GEngine->AddOnScreenDebugMessage( -1 , 0.5f , FColor::Yellow , TEXT( "added" ) );
		}
	}
}

void ADefenseTower::OnSphereEndOverlap( UPrimitiveComponent* OverlappedComponent , AActor* OtherActor , UPrimitiveComponent* OtherComp , int32 OtherBodyIndex )
{
	auto enemy = Cast<AZombieBase>( OtherActor );
	if (enemy && DetectedEnemies.Contains( enemy ))
	{
		DetectedEnemies.Remove( enemy );
		GEngine->AddOnScreenDebugMessage( -1 , 0.5f , FColor::Blue , TEXT( "removed" ) );
	}
}

