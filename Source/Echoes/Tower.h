#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Tower.generated.h"

UCLASS(Blueprintable)
class ECHOES_API ATower : public AActor
{
    GENERATED_BODY()
	
public:	
    ATower();

    UPROPERTY(VisibleAnywhere, Category="Tower")
    UStaticMeshComponent* MeshComponent;

    UPROPERTY(EditAnywhere, Category="Tower")
    float Range = 500.f;

    UPROPERTY(EditAnywhere, Category="Tower")
    float FireRate = 1.0f;

    UPROPERTY(EditAnywhere, Category="Tower")
    float Damage = 10.f;

    FTimerHandle FireTimer;

    UFUNCTION(BlueprintNativeEvent, Category="Tower")
    void Fire();
    virtual void Fire_Implementation();

protected:
    virtual void BeginPlay() override;

public:	
    virtual void Tick(float DeltaTime) override;
};