#include "Tower.h"
#include "Components/StaticMeshComponent.h"

ATower::ATower()
{
    PrimaryActorTick.bCanEverTick = true;

    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
    RootComponent = MeshComponent;
    MeshComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
}

void ATower::BeginPlay()
{
    Super::BeginPlay();
    GetWorldTimerManager().SetTimer(
        FireTimer,
        this,
        &ATower::Fire,
        FireRate,
        true
    );
}

void ATower::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}
    
void ATower::Fire_Implementation()
{
    // comportamento di default, tutte le torri sparano così
    // se una torre figlia fa override in Blueprint, questo non viene chiamato
    UE_LOG(LogTemp, Warning, TEXT("Tower firing!"));
}