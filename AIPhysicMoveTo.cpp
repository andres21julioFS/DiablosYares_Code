// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Possessed/AIPhysicMoveTo.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "DrawDebugHelpers.h"
#include "Characters/PhysicMotion.h"
#include "AIController.h"
#include "NavFilters/NavigationQueryFilter.h"
#include "Components/PrimitiveComponent.h"
// Sets default values for this component's properties
UAIPhysicMoveTo::UAIPhysicMoveTo()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	PhysicMotion = CreateDefaultSubobject<UPhysicMotion>(TEXT("PhysicMotion"));
	// ...
}


// Called when the game starts
void UAIPhysicMoveTo::BeginPlay()
{
	Super::BeginPlay();

	//
	// ...

}

void UAIPhysicMoveTo::SetPrimitiveComponent(UPrimitiveComponent* _PrimitiveComponent, UPrimitiveComponent* RotationComponent, float _offsetRotationAngle)
{
	PrimitiveComponent = _PrimitiveComponent;
	PhysicMotion->SetPrimitiveComponent(PrimitiveComponent, RotationComponent, _offsetRotationAngle);
}
// Called every frame
void UAIPhysicMoveTo::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	return;
	if (TargetActor || ThereIsTargetLocation) {
		UpdateMotion();
	}
	else {
		FRotator Rotation = PrimitiveComponent->GetComponentRotation();
		PrimitiveComponent->SetWorldRotation(FRotator(0, Rotation.Yaw, 0));
	}
}

void UAIPhysicMoveTo::UpdateMotion() {

	FVector EndTargetLocation = ThereIsTargetLocation ? TargetLocation : TargetActor->GetActorLocation();
	FVector Dir = GetDirection(EndTargetLocation);
	float distance = FVector::Distance(EndTargetLocation, PrimitiveComponent->GetComponentLocation());
	FMotionParams motionParams = PhysicMotion->GetMotionParams();
	float distanceToStop = GetDistanceToStop(0, PrimitiveComponent->GetPhysicsLinearVelocity().Length(), motionParams.decceleration);

	float normalizedSpeed = distance <= distanceToStop + DistanceToStopOffset + 50 ? 0 : 1;
	if (distance <= DistanceToStopOffset + 50) {

		ThereIsTargetLocation = false;
		TargetActor = nullptr;
		OnReachTargetLocation.Broadcast();
	}

	PhysicMotion->UpdateMotion(normalizedSpeed, Dir, Dir, true, true, true);
}
float UAIPhysicMoveTo::GetDistanceToStop(float v0, float v, float a) {
	return ((v * v) - (v0 * v0)) / (2 * a);
}
void UAIPhysicMoveTo::SetDistanceToStop(float value) {
	DistanceToStopOffset = value;
}
void UAIPhysicMoveTo::StartFollowActor(AActor* _TargetActor) {
	TargetActor = _TargetActor;
	ThereIsTargetLocation = false;
}
void UAIPhysicMoveTo::SetController(AAIController* Controller) {
	MyController = Controller;
}
FVector UAIPhysicMoveTo::GetDirection(FVector _TargetLocation) {

	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
	FVector Location = PrimitiveComponent->GetComponentLocation();
	UNavigationPath* tpath = nullptr;
	tpath = UNavigationSystemV1::FindPathToLocationSynchronously(GetWorld(), Location, _TargetLocation, GetOwner(), NavigationFilterClass);

	if (tpath != NULL)
	{
		for (int pointiter = 0; pointiter < tpath->PathPoints.Num(); pointiter++)
		{
			DrawDebugSphere(GetWorld(), tpath->PathPoints[pointiter], 10.0f, 12, FColor(255, 0, 0));

		}
		if (tpath->PathPoints.Num() > 1) {
			FVector Dir = tpath->PathPoints[1] - Location;
			Dir.Z = 0;
			Dir.Normalize();
			return Dir;
		}
	}
	return FVector::Zero();
}
void UAIPhysicMoveTo::StartMoveToLocation(FVector _TargetLocation) {
	TargetLocation = _TargetLocation;
	ThereIsTargetLocation = true;
}
void UAIPhysicMoveTo::Enable() {
	this->SetComponentTickEnabled(true);
}
void UAIPhysicMoveTo::Disable() {
	this->SetComponentTickEnabled(false);
}

