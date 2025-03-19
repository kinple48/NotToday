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

	UPROPERTY(EditDefaultsOnly, Category =Input)
	class UInputMappingContext* IMC_Player;

	UPROPERTY(EditDefaultsOnly, Category = Input)
	class UInputAction* IA_Move;
	FVector Direction;
	void Move(const struct FInputActionValue& InputValue);

public:
	UPROPERTY(EditDefaultsOnly, Category = Input)
	class UInputAction* IA_Mouse;

	void Turn(const struct FInputActionValue& InputValue);

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
};
