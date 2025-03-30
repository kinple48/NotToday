// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DefenseTower.generated.h"

UCLASS()
class NOTTODAY_API ADefenseTower : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADefenseTower();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UBoxComponent* boxcomp;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	class UStaticMeshComponent* meshcomp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	class UNavModifierComponent* navModComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HPMax = 10.f;

	void SetDamage( int32 damage );

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USphereComponent* spherecomp;

	UFUNCTION()
	void OnSphereBeginOverlap( UPrimitiveComponent* OverlappedComponent , AActor* OtherActor , UPrimitiveComponent* OtherComp , int32 OtherBodyIndex , bool bFromSweep , const FHitResult& SweepResult );

	UFUNCTION()
	void OnSphereEndOverlap( UPrimitiveComponent* OverlappedComponent , AActor* OtherActor , UPrimitiveComponent* OtherComp , int32 OtherBodyIndex );

	UPROPERTY()
	TArray<AActor*> DetectedEnemies;

	float CurrentTime = 0.0f;
	float AttackDelayTime = 2.0f;
};
