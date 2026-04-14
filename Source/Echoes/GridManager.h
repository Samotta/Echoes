#pragma once

#include "NavigationSystem.h"
#include "NavMesh/NavMeshBoundsVolume.h"
#include "Engine/StaticMeshActor.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridManager.generated.h"

class ATower;

USTRUCT(BlueprintType)
struct FGridCell
{
    GENERATED_BODY()

public:
    // Value initially negative to prevent false positives from TrySelectCell
    UPROPERTY(BlueprintReadWrite, Category = "Grid")
    int32 Row = -1;

    UPROPERTY(BlueprintReadWrite, Category = "Grid")
    int32 Column = -1;

    UPROPERTY(BlueprintReadWrite, Category = "Grid")
    bool bIsWalkable = false;

    UPROPERTY(BlueprintReadWrite, Category = "Grid")
    bool bIsOccupied = false;

    UPROPERTY(BlueprintReadWrite, Category = "Grid")
    bool bIsStart = false;

    UPROPERTY(BlueprintReadWrite, Category = "Grid")
    bool bIsEnd = false;

    UPROPERTY(BlueprintReadWrite, Category = "Grid")
    FVector WorldPosition = FVector::ZeroVector;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCellSelected, FGridCell, Cell);

UCLASS()
class ECHOES_API AGridManager : public AActor
{
    GENERATED_BODY()

public:
    AGridManager();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
    int32 GridRows = 10;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
    int32 GridColumns = 20;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
    float CellSize = 100.f;

    UPROPERTY(BlueprintReadOnly, Category = "Grid")
    TArray<FGridCell> GridCells;

    UPROPERTY(BlueprintReadOnly, Category = "Grid")
    FVector StartPoint;

    UPROPERTY(BlueprintReadOnly, Category = "Grid")
    FVector EndPoint;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid|Path")
    TArray<FIntPoint> PathCoordinates; 

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid|Path")
    int32 PathSeed = 0;

    // Exact number of cells in the generated path
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid|Path")
    int32 PathLength = 20;

    // Probability (0-100) of changing direction at each step
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid|Path", meta = (ClampMin = "0", ClampMax = "100"))
    int32 DirectionChangeProbability = 25;

    // Minimum steps to walk in the same direction before allowing a turn
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid|Path", meta = (ClampMin = "1"))
    int32 MinStraightSteps = 3;

    // Number of direction changes along the path (shapes the path like waypoints, without connectivity issues)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid|Path", meta = (ClampMin = "1", ClampMax = "12"))
    int32 NumTurns = 4;

    UPROPERTY(BlueprintAssignable, Category = "Grid|Selection")
    FOnCellSelected OnCellSelected;

    UPROPERTY(BlueprintReadOnly, Category = "Grid|Selection")
    FIntPoint HoveredCell = FIntPoint(-1, -1);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Tower")
    TSubclassOf<ATower> TowerClass;

    // Used as indicator of where the tower will be placed, follows the mouse cursor
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Tower")
    TSubclassOf<ATower> GhostTowerClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Tower")
    TObjectPtr<ATower> GhostTowerInstance;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Navigation")
    ANavMeshBoundsVolume* NavMeshVolume; // must be assigned in the editor

    // Assign Engine/BasicShapes/Cube in the editor.
    // Cubes are spawned on non-walkable cells to block the NavMesh.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Navigation")
    UStaticMesh* NavBlockerMesh;

    // Called by the player controller when the player clicks on the grid
    UFUNCTION(BlueprintCallable, Category = "Grid|Selection")
    bool TrySelectCell(FVector WorldPosition);

    UFUNCTION(BlueprintCallable, Category = "Grid|Selection")
    bool PlaceTower(FVector WorldPosition);

    UFUNCTION(BlueprintCallable, Category = "Grid")
    void GenerateGrid();

    UFUNCTION(BlueprintCallable, Category = "Grid")
    FGridCell GetCell(int32 Row, int32 Column) const;

    UFUNCTION(BlueprintCallable, Category = "Grid")
    FGridCell GetCellAtWorldPosition(FVector WorldPosition) const;

    UFUNCTION(BlueprintCallable, Category = "Grid")
    void SetCellOccupied(int32 Row, int32 Column, bool bOccupied);

    // Build path from coordinates and sets them as walkable
    UFUNCTION(BlueprintCallable, Category = "Grid|Path")
    void BuildPath();

    UFUNCTION(BlueprintCallable, Category = "Grid|Path")
    TArray<FVector> GetPathWaypoints() const;

    UFUNCTION(BlueprintCallable, Category = "Grid|Path")
    void GenerateProceduralPath(FRandomStream& Rand, FIntPoint Start);

    UFUNCTION(BlueprintCallable, Category = "Navigation")
    void GenerateNavMesh();

protected:
    virtual void BeginPlay() override;

private:
    void DrawDebugGrid() const;
    bool IsClearNeighbor(const FIntPoint& Candidate, const FIntPoint& From, const TSet<FIntPoint>& Visited) const;
    bool DFSWalk(TArray<FIntPoint>& Path, TSet<FIntPoint>& Visited,
                 const TArray<FIntPoint>& SegmentDirs, const TArray<int32>& SegmentLengths,
                 int32 SegmentIndex, int32 StepsInSegment, FRandomStream& Rand) const;

    // Blocker cubes spawned on non-walkable cells to prevent NavMesh from covering them
    UPROPERTY()
    TArray<AStaticMeshActor*> NavBlockers;
};