// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIController.generated.h"

/**
 * 
 */
UCLASS()
class ECHOES_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()

public:
	void StartPathing(const TArray<FVector>& Waypoints);
	
protected:
	 virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;

private:
	TArray<FVector> PathWaypoints;
	int32 CurrentWaypointIndex = 0;

	void MoveToNextWaypoint();
	
};
