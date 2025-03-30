// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Barricade.generated.h"

UCLASS()
class NOTTODAY_API ABarricade : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABarricade();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	int32 price = 200;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	class UBoxComponent* boxcomp;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	class UStaticMeshComponent* meshcomp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	class UNavModifierComponent* navModComp;

	//체력 시스템
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HPMax = 10.f;

	void SetDamage( int32 damage );
};
