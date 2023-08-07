// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AIPhysicMoveTo.generated.h"

class UPhysicMotion;
class UPrimitiveComponent;
class AAIController;
class UNavigationQueryFilter;


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FReachTargetLocation);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DIABLOSYARES_API UAIPhysicMoveTo : public UActorComponent
{
	GENERATED_BODY()
		FVector GetDirection(FVector TargetLocation);
	void UpdateMotion();
	AActor* TargetActor;
	FVector TargetLocation;
	bool ThereIsTargetLocation;
	AAIController* MyController;
	FTimerHandle UpdateMotionHandle;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UPhysicMotion* PhysicMotion;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		TSubclassOf<UNavigationQueryFilter> NavigationFilterClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		UPrimitiveComponent* PrimitiveComponent;
	float GetDistanceToStop(float v0, float v, float a);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		float DistanceToStopOffset;
public:
	// Sets default values for this component's properties
	UAIPhysicMoveTo();
	UFUNCTION(BlueprintCallable)
		void SetPrimitiveComponent(UPrimitiveComponent* _PrimitiveComponent, UPrimitiveComponent* RotationComponent, float _offsetRotationAngle);
	UFUNCTION(BlueprintCallable)
		void StartFollowActor(AActor* _TargetActor);
	UFUNCTION(BlueprintCallable)
		void StartMoveToLocation(FVector TargetLocation);
	void SetController(AAIController* Controller);
	void Enable();
	void Disable();
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void SetDistanceToStop(float value);
	UPROPERTY(BlueprintAssignable)
		FReachTargetLocation OnReachTargetLocation;
};
