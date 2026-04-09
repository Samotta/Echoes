#include "GridManager.h"
#include "DrawDebugHelpers.h"

#include "Algo/Reverse.h"

#include "Tower.h"

AGridManager::AGridManager()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AGridManager::BeginPlay()
{
    Super::BeginPlay();
    GenerateGrid();

    FRandomStream Rand;
    Rand = (PathSeed == 0) ? FRandomStream(FMath::Rand()) : FRandomStream(PathSeed);

    FIntPoint Start = FIntPoint(Rand.RandRange(1, GridColumns - 2), Rand.RandRange(1, GridRows - 2));

    GenerateProceduralPath(Rand, Start);
}

void AGridManager::GenerateGrid()
{
    GridCells.Empty();

    for (int32 Row = 0; Row < GridRows; Row++)
    {
        for (int32 Col = 0; Col < GridColumns; Col++)
        {
            FGridCell Cell;
            Cell.Row = Row;
            Cell.Column = Col;
            Cell.bIsWalkable = false;
            Cell.bIsOccupied = false;
            Cell.bIsStart = false;
            Cell.bIsEnd = false;

            Cell.WorldPosition = GetActorLocation() + FVector(
                Col * CellSize,
                Row * CellSize,
                0.f
            );

            GridCells.Add(Cell);
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("GridManager: generated grid with %d cells (%d x %d)"),
        GridCells.Num(), GridRows, GridColumns);
}

FGridCell AGridManager::GetCell(int32 Row, int32 Column) const
{
    int32 Index = Row * GridColumns + Column;
    if (GridCells.IsValidIndex(Index))
        return GridCells[Index];

    UE_LOG(LogTemp, Error, TEXT("GetCell: index out of bounds [%d, %d]"), Row, Column);
    return FGridCell();
}

FGridCell AGridManager::GetCellAtWorldPosition(FVector WorldPosition) const
{
    FVector LocalPos = WorldPosition - GetActorLocation();
    int32 Col = FMath::FloorToInt(LocalPos.X / CellSize);
    int32 Row = FMath::FloorToInt(LocalPos.Y / CellSize);

    if (Col < 0 || Col >= GridColumns || Row < 0 || Row >= GridRows)
        return FGridCell();

    return GetCell(Row, Col);
}

void AGridManager::SetCellOccupied(int32 Row, int32 Column, bool bOccupied)
{
    int32 Index = Row * GridColumns + Column;
    if (GridCells.IsValidIndex(Index))
        GridCells[Index].bIsOccupied = bOccupied;
}

void AGridManager::DrawDebugGrid() const
{
    for (const FGridCell& Cell : GridCells)
    {
        FVector Center = Cell.WorldPosition + FVector(CellSize * 0.5f, CellSize * 0.5f, 0.f);
        FVector Extent = FVector(CellSize * 0.5f - 2.f, CellSize * 0.5f - 2.f, 5.f);

        FColor Color = Cell.bIsWalkable ? FColor::Green : FColor::Blue;
        if (Cell.bIsStart)
            Color = FColor::Yellow;
        else if (Cell.bIsEnd)
            Color = FColor::Red;
        DrawDebugBox(GetWorld(), Center, Extent, Color, true, -1.f, 0, 2.f);
    }
}

void AGridManager::BuildPath()
{
    for (FGridCell& Cell : GridCells)
    {
        Cell.bIsWalkable = false;
        Cell.bIsStart = false;
        Cell.bIsEnd = false;
    }

    for (const FIntPoint& Coord : PathCoordinates)
    {
        int32 Index = Coord.Y * GridColumns + Coord.X;
        if (GridCells.IsValidIndex(Index))
            GridCells[Index].bIsWalkable = true;
        else
            UE_LOG(LogTemp, Warning, TEXT("BuildPath: coordinate out of bounds [%d, %d]"), Coord.X, Coord.Y);
    }

    if (PathCoordinates.Num() > 0)
    {
        int32 StartIndex = PathCoordinates[0].Y * GridColumns + PathCoordinates[0].X;
        int32 EndIndex   = PathCoordinates.Last().Y * GridColumns + PathCoordinates.Last().X;

        if (GridCells.IsValidIndex(StartIndex)) GridCells[StartIndex].bIsStart = true;
        if (GridCells.IsValidIndex(EndIndex))   GridCells[EndIndex].bIsEnd = true;
    }

    DrawDebugGrid();
}

TArray<FVector> AGridManager::GetPathWaypoints() const
{
    TArray<FVector> Waypoints;

    for (const FIntPoint& Coord : PathCoordinates)
    {
        int32 Index = Coord.Y * GridColumns + Coord.X;
        if (GridCells.IsValidIndex(Index))
            Waypoints.Add(GridCells[Index].WorldPosition + FVector(CellSize * 0.5f, CellSize * 0.5f, 0.f));
    }

    return Waypoints;
}

bool AGridManager::IsClearNeighbor(const FIntPoint& Candidate, const FIntPoint& From, const TSet<FIntPoint>& Visited) const
{
    const FIntPoint Dirs[] = {
        FIntPoint(1, 0), FIntPoint(-1, 0),
        FIntPoint(0, 1), FIntPoint(0, -1)
    };
    for (const FIntPoint& Dir : Dirs)
    {
        FIntPoint N(Candidate.X + Dir.X, Candidate.Y + Dir.Y);
        if (N == From) continue;
        if (Visited.Contains(N)) return false;
    }
    return true;
}

// DFS with full backtracking — guaranteed to find a path of exactly PathLength cells
// if the grid has enough free space. Uses segment bias for natural-looking turns.
bool AGridManager::DFSWalk(TArray<FIntPoint>& Path, TSet<FIntPoint>& Visited,
                            const TArray<FIntPoint>& SegmentDirs,
                            const TArray<int32>& SegmentLengths,
                            int32 SegmentIndex, int32 StepsInSegment,
                            FRandomStream& Rand) const
{
    // Base case: path is complete
    if (Path.Num() == PathLength + 1)
        return true;

    const FIntPoint Dirs[] = {
        FIntPoint(1, 0), FIntPoint(-1, 0),
        FIntPoint(0, 1), FIntPoint(0, -1)
    };

    // Advance segment if current one is done
    int32 CurrentSegment = SegmentIndex;
    int32 CurrentSteps   = StepsInSegment;
    while (CurrentSegment < SegmentDirs.Num() - 1 && CurrentSteps >= SegmentLengths[CurrentSegment])
    {
        CurrentSegment++;
        CurrentSteps = 0;
    }

    FIntPoint BiasDir = SegmentDirs[CurrentSegment];
    FIntPoint Current = Path.Last();

    // Build candidate list: biased first, then lateral, shuffled within each group
    TArray<FIntPoint> Biased, Lateral;
    for (const FIntPoint& Dir : Dirs)
    {
        FIntPoint Next(Current.X + Dir.X, Current.Y + Dir.Y);
        if (Next.X < 0 || Next.X >= GridColumns) continue;
        if (Next.Y < 0 || Next.Y >= GridRows)    continue;
        if (Visited.Contains(Next))              continue;
        if (!IsClearNeighbor(Next, Current, Visited)) continue;

        if (Dir == BiasDir)
            Biased.Add(Next);
        else
            Lateral.Add(Next);
    }

    // Shuffle each group independently so we don't always try the same order
    for (int32 i = Biased.Num() - 1; i > 0; i--)
        Biased.Swap(i, Rand.RandRange(0, i));
    for (int32 i = Lateral.Num() - 1; i > 0; i--)
        Lateral.Swap(i, Rand.RandRange(0, i));

    // Try biased candidates first (DirectionChangeProbability % weight → put them at the front),
    // then lateral.
    TArray<FIntPoint> Candidates;
    if (Rand.RandRange(0, 99) < 100 - DirectionChangeProbability)
    {
        Candidates.Append(Biased);
        Candidates.Append(Lateral);
    }
    else
    {
        Candidates.Append(Lateral);
        Candidates.Append(Biased);
    }

    for (const FIntPoint& Next : Candidates)
    {
        Path.Add(Next);
        Visited.Add(Next);

        int32 NewSteps = CurrentSteps + 1;
        if (DFSWalk(Path, Visited, SegmentDirs, SegmentLengths, CurrentSegment, NewSteps, Rand))
            return true;

        // Backtrack
        Path.Pop();
        Visited.Remove(Next);
    }

    return false; // no valid move found — caller will backtrack further
}

void AGridManager::GenerateProceduralPath(FRandomStream& Rand, FIntPoint Start)
{
    UE_LOG(LogTemp, Warning, TEXT("Building Path"));
    PathCoordinates.Empty();

    const FIntPoint Dirs[] = {
        FIntPoint(1, 0), FIntPoint(-1, 0),
        FIntPoint(0, 1), FIntPoint(0, -1)
    };

    // Split PathLength into NumTurns segments
    TArray<int32> SegmentLengths;
    {
        int32 Base = PathLength / NumTurns;
        int32 Remainder = PathLength % NumTurns;
        for (int32 i = 0; i < NumTurns; i++)
            SegmentLengths.Add(Base + (i < Remainder ? 1 : 0));
    }

    // Assign one bias direction per segment (no 180° reversals)
    TArray<FIntPoint> SegmentDirs;
    {
        FIntPoint LastDir = Dirs[Rand.RandRange(0, 3)];
        SegmentDirs.Add(LastDir);
        for (int32 i = 1; i < NumTurns; i++)
        {
            FIntPoint Reverse(-LastDir.X, -LastDir.Y);
            TArray<FIntPoint> Candidates;
            for (const FIntPoint& D : Dirs)
                if (D != Reverse) Candidates.Add(D);
            LastDir = Candidates[Rand.RandRange(0, Candidates.Num() - 1)];
            SegmentDirs.Add(LastDir);
        }
    }

    TSet<FIntPoint> Visited;
    Visited.Add(Start);
    PathCoordinates.Add(Start);

    bool bSuccess = DFSWalk(PathCoordinates, Visited, SegmentDirs, SegmentLengths, 0, 0, Rand);

    if (!bSuccess)
    {
        UE_LOG(LogTemp, Warning, TEXT("GenerateProceduralPath: could not reach PathLength %d — grid too small or PathLength too large. Got %d cells."),
            PathLength, PathCoordinates.Num() - 1);
    }

    UE_LOG(LogTemp, Warning, TEXT("Path generated: %d/%d cells, %d turns, start [%d,%d] end [%d,%d]"),
        PathCoordinates.Num() - 1, PathLength, NumTurns,
        Start.X, Start.Y,
        PathCoordinates.Last().X, PathCoordinates.Last().Y);

    BuildPath();
}


bool AGridManager::TrySelectCell(FVector WorldPosition)
{
    FGridCell Cell = GetCellAtWorldPosition(WorldPosition);

    int32 Index = Cell.Row * GridColumns + Cell.Column;
    if (!GridCells.IsValidIndex(Index))
        return false;

    if (GridCells[Index].bIsWalkable || GridCells[Index].bIsOccupied)
        return false;

    OnCellSelected.Broadcast(GridCells[Index]);
    UE_LOG(LogTemp, Warning, TEXT("Cell selected: [%d, %d]"), Cell.Row, Cell.Column);
    return true;
}

bool AGridManager::PlaceTower(FVector WorldPosition)
{
    if (!TowerClass.Get()) return false;
    if (!TrySelectCell(WorldPosition)) return false;

    FGridCell Cell = GetCellAtWorldPosition(WorldPosition);

    int32 Index = Cell.Row * GridColumns + Cell.Column;

    // Center of the cell
    FVector Center = FVector::ZeroVector;
    Center.X = Cell.WorldPosition.X + CellSize * 0.5f;
    Center.Y = Cell.WorldPosition.Y + CellSize * 0.5f;

    // Spawn
    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = this;

    ATower* SpawnedTower = GetWorld()->SpawnActor<ATower>(
        TowerClass.Get(),
        Center,
        FRotator::ZeroRotator,
        SpawnParams
    );

    if (SpawnedTower)
    {
        GridCells[Index].bIsOccupied = true;
        return true;
    }

    return false;
}