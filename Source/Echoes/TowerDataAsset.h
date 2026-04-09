#pragma once
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Tower.h"
#include "TowerDataAsset.generated.h"

USTRUCT(BlueprintType)
struct FTowerData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Tower")
    TSubclassOf<ATower> TowerClass;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Tower")
    TSubclassOf<ATower> GhostTowerClass;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Tower")
    FText Name;
};

UCLASS()
class ECHOES_API UTowerDataAsset : public UPrimaryDataAsset
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Towers")
    TArray<FTowerData> Towers;
};