// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAIController.h"
#include "Navigation/PathFollowingComponent.h"

void AEnemyAIController::StartPathing(const TArray<FVector>& Waypoints)
{
    PathWaypoints = Waypoints;
    CurrentWaypointIndex = 0;

    // // Draw debug
    // for (int32 i = 0; i < Waypoints.Num(); i++)
    // {
    //     // Sfera su ogni waypoint
    //     DrawDebugSphere(
    //         GetWorld(),
    //         Waypoints[i],
    //         30.f,           // radius
    //         12,             // segments
    //         FColor::Cyan,
    //         true,           // persistent
    //         -1.f
    //     );

        
    //     DrawDebugString(
    //         GetWorld(),
    //         Waypoints[i] + FVector(0, 0, 50.f),
    //         FString::Printf(TEXT("%d"), i),
    //         nullptr,
    //         FColor::White,
    //         -1.f,
    //         true
    //     );

        
    //     if (i > 0)
    //     {
    //         DrawDebugLine(
    //             GetWorld(),
    //             Waypoints[i - 1],
    //             Waypoints[i],
    //             FColor::Orange,
    //             true,
    //             -1.f,
    //             0,
    //             3.f
    //         );
    //     }
    // }

    MoveToNextWaypoint();
}

void AEnemyAIController::MoveToNextWaypoint()
{
    if (!PathWaypoints.IsValidIndex(CurrentWaypointIndex))
    {
        // Enemy has reached the end of the path
        if (APawn* ControlledPawn = GetPawn())
        {
            //TODO: actions on reaching the end of the path, e.g. lower tower health, etc.
            ControlledPawn->Destroy();
        }
        return;
    }

    MoveToLocation(PathWaypoints[CurrentWaypointIndex], 
                    /*AcceptanceRadius*/    100.f, 
                    /*bStopOnOverlap*/      false, 
                    /*bUsePathfinding*/     true);
}

void AEnemyAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
    Super::OnMoveCompleted(RequestID, Result);

    if (Result.IsSuccess())
    {
        CurrentWaypointIndex++;
        MoveToNextWaypoint();
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("EnemyAI: path blocked at waypoint %d"), CurrentWaypointIndex);
    }
}
