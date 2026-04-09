#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IsometricCamera.generated.h"

UCLASS()
class ECHOES_API AIsometricCamera : public AActor
{
    GENERATED_BODY()

public:
    AIsometricCamera();

    // Angle around the grid in degrees (0-360)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    float OrbitAngle = 45.f;

    // Elevation angle from horizontal (degrees)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    float ElevationAngle = 50.f;

    // Distance from grid center
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    float OrbitDistance = 2000.f;

    // Rotation speed (degrees per second)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    float RotationSpeed = 90.f;

    // Reference to GridManager to compute center
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    class AGridManager* GridManager;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera")
	float MinOrbitDistance = 300.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera")
	float MaxOrbitDistance = 2000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera")
	float ZoomSpeed = 200.f;

    // Rotate left/right (-1, 0, 1)
    UFUNCTION(BlueprintCallable, Category = "Camera")
    void AddOrbitInput(float Direction);

    // Snap camera to grid center and current angle
    UFUNCTION(BlueprintCallable, Category = "Camera")
    void UpdateCameraPosition();

	UFUNCTION(BlueprintCallable, Category = "Camera")
	void AddZoomInput(float DeltaZoom);

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

private:
    UPROPERTY(VisibleAnywhere)
    class UCameraComponent* CameraComponent;

    UPROPERTY(VisibleAnywhere)
    class USpringArmComponent* SpringArm;

    float OrbitInput = 0.f;
	float ZoomInput = 0.f;
    FVector GridCenter = FVector::ZeroVector;
};