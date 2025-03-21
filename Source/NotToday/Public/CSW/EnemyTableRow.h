#pragma once

#include "CoreMinimal.h" // 먼저 포함
#include "Engine/DataTable.h" // 이후에 포함
#include "Engine/SkeletalMesh.h"
#include "Engine/StaticMesh.h"

#include "EnemyTableRow.generated.h"

USTRUCT()
struct FEnemyTableRow : public FTableRowBase
{
	GENERATED_BODY() // GENERATED_USTRUCT_BODY() 대신 사용

public:
	UPROPERTY(EditAnywhere, Category = "Enemy")
	FText DisplayName;

	UPROPERTY(EditAnywhere, Category = "Enemy")
	USkeletalMesh* SkeletalMesh;

	UPROPERTY(EditAnywhere, Category = "Enemy")
	UStaticMesh* StaticMesh;

	UPROPERTY(EditAnywhere, Category = "Enemy")
	FTransform OffsetTransform;
};