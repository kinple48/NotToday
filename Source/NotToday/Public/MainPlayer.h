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

public:
	UPROPERTY(EditDefaultsOnly, Category = Input)
	class UInputAction* IA_GunShot;
	bool bGunShot = false;
	void GunShotStart(const struct FInputActionValue& InputValue);
	void GunShotEnd(const struct FInputActionValue& InputValue);
	void GunShot();
	float CurrentTime = 0.5f;
	float MakeTime = 0.5f;

	UPROPERTY(EditAnywhere, Category = BulletEffact)
	class UParticleSystem* BulletEffactFactory;

	UPROPERTY()
	class USoundBase* BulletSound;

	float state = 0;
	float walkspeed = 600.f;

	UPROPERTY()
	class ASpawnPoint* Spawnpoint;

	UPROPERTY()
	class ASpawnPoint* Tmp_Spawnpoint;

	UFUNCTION()
	void OnSpawnPointBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnSpawnPointEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY()
    TMap<int32, AActor*> OverlapMap;
	
	int32 CalculateSlotIndex(ASpawnPoint* Actor);

	AActor* FindClosestActorToPlayer();

	void OverlapEvent( ASpawnPoint* Spawnpoint, AMainPlayer* player );

	UPROPERTY(EditAnywhere)
	TSubclassOf<class ABarricade> BarricadeFactory;

	void RotateToMouseCursor();
	void SpawnObject();

	UPROPERTY(EditDefaultsOnly)
	class USkeletalMeshComponent* GunMesh;

	UPROPERTY(EditAnywhere)
	int32 CashData = 90000;

	UPROPERTY(EditAnywhere)
	int32 BarricadeStoreData = 0;

	UPROPERTY(EditAnywhere)
	int32 BarricadePrice = 200;

	UPROPERTY(EditAnywhere)
	class ABarricade* BarricadeObject;

	class AMainGameModeBase* GameMode;

	UPROPERTY()
	bool CombatState = false;
};
