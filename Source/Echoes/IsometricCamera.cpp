#include "IsometricCamera.h"
#include "GridManager.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"

AIsometricCamera::AIsometricCamera()
{
    PrimaryActorTick.bCanEverTick = true;
	
    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArm->SetupAttachment(RootComponent);
    SpringArm->TargetArmLength = 0.f; // we control position manually
    SpringArm->bDoCollisionTest = false;

    CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    CameraComponent->SetupAttachment(SpringArm);
    CameraComponent->SetProjectionMode(ECameraProjectionMode::Perspective);
    CameraComponent->FieldOfView = 60.f;
}

void AIsometricCamera::BeginPlay()
{
    Super::BeginPlay();

    // Auto-find GridManager if not set
    if (!GridManager)
    {
        GridManager = Cast<AGridManager>(
            UGameplayStatics::GetActorOfClass(GetWorld(), AGridManager::StaticClass())
        );
    }

    if (GridManager)
    {
        // Compute grid center from dimensions
        float GridWidth  = GridManager->GridColumns * GridManager->CellSize;
        float GridHeight = GridManager->GridRows    * GridManager->CellSize;
        GridCenter = GridManager->GetActorLocation() + FVector(GridWidth * 0.5f, GridHeight * 0.5f, 0.f);
    }

    UpdateCameraPosition();
}

void AIsometricCamera::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

	// Update camera rotation
    if (FMath::Abs(OrbitInput) > 0.01f)
    {
        OrbitAngle += OrbitInput * RotationSpeed * DeltaTime;
        OrbitAngle = FMath::Fmod(OrbitAngle, 360.f);
        if (OrbitAngle < 0.f) OrbitAngle += 360.f;

        OrbitInput = 0.f;
    }

	// Update camera zoom
	if (FMath::Abs(ZoomInput) > 0.01f)
    {
        OrbitDistance = FMath::Clamp(OrbitDistance + ZoomInput * ZoomSpeed * DeltaTime,
                                     MinOrbitDistance, MaxOrbitDistance);
        ZoomInput = 0.f;  // reset per frame
    }

	UpdateCameraPosition();
}

void AIsometricCamera::AddOrbitInput(float Direction)
{
    OrbitInput = FMath::Clamp(Direction, -1.f, 1.f);
}

void AIsometricCamera::UpdateCameraPosition()
{
    // Convert orbit + elevation angles to a position on a sphere
    float OrbitRad    = FMath::DegreesToRadians(OrbitAngle);
    float ElevRad     = FMath::DegreesToRadians(ElevationAngle);

    FVector Offset;
    Offset.X = OrbitDistance * FMath::Cos(ElevRad) * FMath::Cos(OrbitRad);
    Offset.Y = OrbitDistance * FMath::Cos(ElevRad) * FMath::Sin(OrbitRad);
    Offset.Z = OrbitDistance * FMath::Sin(ElevRad);

    SetActorLocation(GridCenter + Offset);

    // Always look at grid center
    FVector LookDir = (GridCenter - GetActorLocation()).GetSafeNormal();
    SetActorRotation(LookDir.Rotation());
}

void AIsometricCamera::AddZoomInput(float DeltaZoom)
{
	ZoomInput = FMath::Clamp(DeltaZoom, -1.f, 1.f);
}
