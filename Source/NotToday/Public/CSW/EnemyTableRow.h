#pragma once

#include "CoreMinimal.h" // 먼저 포함
#include "Engine/DataTable.h" // 이후에 포함
#include "Engine/SkeletalMesh.h"
#include "Engine/StaticMesh.h"

#include "EnemyTableRow.generated.h"

USTRUCT()
struct FEnemyTableRow : public FTableRowBase
{
	GENERATED_BODY()

	FEnemyTableRow()
		: MaxWalkSpeed(0.0f) , MaxHP(0) , Damage(0), AttackDelayTime(0.0f), AttackRange(0.0f)
	{ }
	
	UPROPERTY(EditAnywhere, Category = "Enemy")
	FText DisplayName;

	UPROPERTY(EditAnywhere, Category = "Enemy")
	TObjectPtr<USkeletalMesh> SkeletalMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Enemy | Anim")
	TObjectPtr<UAnimBlueprint> AnimBP;

	UPROPERTY(EditDefaultsOnly, Category = "Enemy | Anim")
	TObjectPtr<UAnimMontage> Montage;
	
	UPROPERTY(EditDefaultsOnly, Category = "Enemy")
	FTransform MeshOffsetTransform;

	UPROPERTY(EditDefaultsOnly, Category = "Enemy")
	float MaxWalkSpeed;
	
	UPROPERTY(EditDefaultsOnly, Category = "Enemy")
	int32 MaxHP;
	
	UPROPERTY(EditDefaultsOnly, Category = "Enemy")
	int32 Damage; // 공격력

	UPROPERTY(EditDefaultsOnly, Category = "Enemy")
	float AttackDelayTime;

	UPROPERTY(EditDefaultsOnly, Category = "Enemy")
	float AttackRange; // 사거리
	

}; 