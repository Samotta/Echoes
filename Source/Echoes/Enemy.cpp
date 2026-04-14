// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "GridManager.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AEnemy::AEnemy()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AIControllerClass = AEnemyAIController::StaticClass();
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	MovementComponent = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("MovementComponent"));
	MovementComponent->MaxSpeed = Speed;

	UE_LOG(LogTemp, Warning, TEXT("Enemy constructor: Speed set to %f"), Speed);

}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	if (GetRootComponent())
    {
        GetRootComponent()->SetCanEverAffectNavigation(false);
    }

	UE_LOG(LogTemp, Warning, TEXT("Enemy BeginPlay: Speed is %f"), Speed);

	if (AGridManager* GM = Cast<AGridManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AGridManager::StaticClass())))
    {
		UE_LOG(LogTemp, Warning, TEXT("Enemy spawned"));
        TArray<FVector> Waypoints = GM->GetPathWaypoints();
        InitializePathing(Waypoints);
    }
	
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::Die()
{
	// Avoid broadcasting multiple times if already dead
	if (IsPendingKillPending())
	{
		return;
	}

	OnDeath.Broadcast();
	Destroy();
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
    Health -= ActualDamage;
    if (Health <= 0.f)
        Die();
    return ActualDamage;
}

void AEnemy::InitializePathing(const TArray<FVector>& Waypoints)
{
    if (AEnemyAIController* AIC = Cast<AEnemyAIController>(GetController()))
    {
        AIC->StartPathing(Waypoints);
    }
}
