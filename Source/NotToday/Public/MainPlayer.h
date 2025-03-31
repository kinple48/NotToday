// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MainPlayer.generated.h"

UCLASS()
class NOTTODAY_API AMainPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMainPlayer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	UPROPERTY(VisibleAnywhere, Category = "MotionController")
	class UMotionControllerComponent* LeftHand;

	UPROPERTY(VisibleAnywhere, Category = "MotionController")
	class UMotionControllerComponent* RightHand;


	UPROPERTY(VisibleAnywhere)
	class UCameraComponent* VRCamera;

	UPROPERTY(VisibleAnywhere)
	class USpringArmComponent* SpringArmComp;

	UPROPERTY(EditDefaultsOnly, Category =Input)
	class UInputMappingContext* IMC_Player;

	UPROPERTY(EditDefaultsOnly, Category = Input)
	class UInputAction* IA_Move;
	FVector Direction;
	FVector Direction_Mouse;
	void Move(const struct FInputActionValue& InputValue);

public:
	UPROPERTY(EditDefaultsOnly, Category = Input)
	class UInputAction* IA_Mouse;

	//------------------------------VR-----------------------------------------------------
	
	//Turn
	UPROPERTY( EditDefaultsOnly , Category = Input )
	class UInputAction* IA_Turn;
	void Turn( const struct FInputActionValue& InputValue );
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Setting)
	float RotationSpeed = 50.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settings")
	float RotationInterpSpeed = 5.0f;

	//UI
	UPROPERTY(EditAnywhere, Category="UI")
	class UWidgetComponent* WidgetComp;
	class UDay_UI* DayUIInstance;
	class UNight_UI* NightUIInstance;

	//--------------------------------------------------------------------------------------

public:
	UPROPERTY(EditDefaultsOnly, Category = Input)
	class UInputAction* IA_GunShot;
	bool bGunShot = false;
	void GunShotStart(const struct FInputActionValue& InputValue);
	void GunShotEnd(const struct FInputActionValue& InputValue);
	void GunShot();

	UPROPERTY(VisibleAnywhere)
	class UChildActorComponent* CrosshairComp;

	void DrawCrossHair();
	float CurrentTime = 0.5f;
	float MakeTime = 0.5f;

	UPROPERTY(EditAnywhere, Category = BulletEffact)
	class UParticleSystem* BulletEffactFactory;

	UPROPERTY(EditAnywhere, Category = BulletEffact)
	class UParticleSystem* ObjectEffectFactory;

	UPROPERTY()
	class USoundBase* BulletSound;

	UPROPERTY()
	class USoundBase* ObjectSound;

	float state = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float walkspeed = 600.f;

	UPROPERTY()
	class ASpawnPoint* Spawnpoint;

	UPROPERTY()
	class ASpawnPoint* Tmp_Spawnpoint;

	UFUNCTION()
	void OnSpawnPointBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnSpawnPointEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void OnDropItemBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY()
    TMap<int32, AActor*> OverlapMap;
	
	int32 CalculateSlotIndex(ASpawnPoint* Actor);

	AActor* FindClosestActorToPlayer();

	void OverlapEvent( ASpawnPoint* Spawnpoint, AMainPlayer* player );

	UPROPERTY(EditAnywhere)
	TSubclassOf<class ABarricade> BarricadeFactory;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class ADefenseTower> AutoTurretFactory;

	void SpawnObject();

	UPROPERTY(EditDefaultsOnly)
	class USkeletalMeshComponent* GunMesh;

	UPROPERTY(EditAnywhere)
	int32 CashData = 9000;

	UPROPERTY(EditAnywhere)
	int32 ScoreData = 0;

	UPROPERTY(EditAnywhere)
	int32 StoreData = 0;

	UPROPERTY(EditAnywhere)
	int32 Price = 200;

	UPROPERTY(EditAnywhere)
	int32 BarricadeStoreData = 0;

	UPROPERTY(EditAnywhere)
	int32 BarricadePrice = 200;

	UPROPERTY(EditAnywhere)
	int32 AutoTurretStoreData = 0;

	UPROPERTY(EditAnywhere)
	int32 AutoTurretPrice = 3000;


	UPROPERTY(EditAnywhere)
	class ABarricade* BarricadeObject;

	UPROPERTY(EditAnywhere)
	class AActor* OverlapActor;
	class AMainGameModeBase* GameMode;

	UPROPERTY()
	bool CombatState = true;


	//체력 시스템
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float HP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HPMax = 10.f;

	void SetDamage( int32 damage );

	//장전 시스템
	float Reload;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ReloadMax = 12.f;
	float ReloadTime = 2.f;

	void SetReload( );

	
	// spawn point mesh 조작
	FVector SpawnLocation = FVector( 0.f , 0.f , 0.f );
	FRotator SpawnRotation = FRotator( 0.f , 0.f , 0.f );
	FVector SpawnScale = FVector( 0.f , 0.f , 0.f );

	UFUNCTION()
	void DayState();

	UFUNCTION()
	void NightState();

	UPROPERTY(EditDefaultsOnly, Category = Input)
	class UInputAction* IA_Buy;
	void Buy( const struct FInputActionValue& InputValue );

	UPROPERTY(EditDefaultsOnly, Category = Input)
	class UInputAction* IA_NextLevel;
	void NextLevel( const struct FInputActionValue& InputValue );

	UPROPERTY(EditDefaultsOnly, Category = Input)
	class UInputAction* IA_Place;
	void Place( const struct FInputActionValue& InputValue );

	UPROPERTY(EditDefaultsOnly, Category = Input)
	class UInputAction* IA_Remove;
	void Remove( const struct FInputActionValue& InputValue );

	UPROPERTY(EditAnywhere)
	bool ObjectType = true;

	void WoodenBarricade();
	void AutoTurret();

	UPROPERTY(EditDefaultsOnly, Category = Input)
	class UInputAction* IA_Object;
	void Object( const struct FInputActionValue& InputValue );
	FTimerHandle TimerHandle;

	float EaseOutBounce( float x );
	float FlippedEaseOutBounce( float x );
	void ApplyBouncingEffect( AActor* TargetActor , FVector SpawnLocation , float BounceHeight , float Duration );
	
};
