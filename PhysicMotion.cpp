// Fill out your copyright notice in the Description page of Project Settings.
#include "Characters/PhysicMotion.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"
#include "FunctionsLibrary/MyMaths.h"
#include "Engine/Engine.h"
#include "DrawDebugHelpers.h"
//#include "FunctionsLibrary/MyMaths.h"
// Sets default values for this component's properties
UPhysicMotion::UPhysicMotion()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UPhysicMotion::BeginPlay()
{
	Super::BeginPlay();
	maxMassLerp = 1 / maxMassLerp;
	// ...

}

void UPhysicMotion::SetPrimitiveComponent(UPrimitiveComponent* PrimitiveComponent, USceneComponent* RotationComponent, float _offsetRotationAngle)
{
	Body = PrimitiveComponent;
	RotationBody = RotationComponent;
	Body->SetSimulatePhysics(true);
	Body->SetEnableGravity(true);
	Body->SetAngularDamping(10000000);
	FBodyInstance* BodyInstance = Body->GetBodyInstance();
	BodyInstance->bLockYRotation = true;
	BodyInstance->bLockXRotation = true;
	BodyInstance->AngularDamping = 1000000;
	initMass = Body->GetMass();
	offsetRotationAngle = _offsetRotationAngle;
}
void UPhysicMotion::UpdateMotion(float NormalizedSpeed, FVector WorldDirection, FVector LookDirection, bool IsOnTheFloor, bool CanMove, bool CanImmovilize)
{
	if (CanMove) {
		if (FMath::IsNearlyZero(NormalizedSpeed, 0.0001f)) {
			if (CanImmovilize) {
				Immovilize(IsOnTheFloor);
				UPhysicMotion::LookToDirection(LookDirection);
			}
		}
		else {
			UPhysicMotion::LookToDirection(LookDirection);
			FVector actorForwardDirection = Body->GetForwardVector();
			actorForwardDirection.Z = 0;
			float angle = FMath::Acos(FVector::DotProduct(actorForwardDirection, WorldDirection));

			float desiredSpeed = GetMaxSpeed(NormalizedSpeed, angle, IsOnTheFloor);
			FVector Force = GetForce(WorldDirection, desiredSpeed, IsOnTheFloor);
			Body->AddForce(Force, "None", false);

		}

	}
	else if (CanImmovilize) {
		Immovilize(IsOnTheFloor);
	}

}
void UPhysicMotion::LookToDirection(FVector WorldDirection)
{
	WorldDirection.Z = 0;
	FMotionParams motionParams = GetMotionParams();
	FVector Forward = RotationBody->GetForwardVector();
	Forward = Forward.RotateAngleAxis(-offsetRotationAngle, FVector::UpVector);
	FVector UpAxis = FVector::CrossProduct(Forward, WorldDirection);
	float maxAngle = UMyMaths::Angle(Forward, WorldDirection);
	float angle = motionParams.rotationSpeed * GetWorld()->GetDeltaSeconds();
	angle = FMath::Clamp(angle, 0, maxAngle);
	FVector Direction = Forward.RotateAngleAxis(angle, UpAxis);
	Direction.Z = 0;
	FRotator Rotation = Direction.Rotation();
	Rotation.Yaw += offsetRotationAngle;
	Rotation.Roll = 0;
	Rotation.Pitch = 0;
	RotationBody->SetWorldRotation(Rotation);
	FRotator BodyRotation = Body->GetComponentRotation();
	Body->SetWorldRotation(FRotator(0, BodyRotation.Yaw, 0));
}
void UPhysicMotion::Immovilize(bool IsOnTheFloor)
{
	if (IsOnTheFloor) {
		FVector Velocity = Body->GetPhysicsLinearVelocity();
		Velocity.Z = 0;
		float deceleration = GetMotionParams().decceleration;
		float maxDeceleration = FMath::Clamp(Velocity.Length(), 0, 100000000000) / GetWorld()->GetDeltaSeconds();
		deceleration = FMath::Clamp(deceleration, 0, maxDeceleration) * Body->GetMass();
		Velocity.Normalize();
		FVector Force = Velocity * deceleration * -1;
		Body->AddForce(Force, "None", false);
	}
}
float UPhysicMotion::GetMaxSpeed(float InputLenght, float angle, bool isOnTheFloor)
{
	FMotionParams motionParams = GetMotionParams();
	if (isOnTheFloor) {
		float a1 = FMath::Clamp(angle / 90, 0, 1);
		float a2 = FMath::Clamp((angle - 90) / 90, 0, 1);
		float result = FMath::Lerp(motionParams.forwardSpeed, motionParams.horizontalSpeed, a1);
		result = FMath::Lerp(result, motionParams.backSpeed, a2);
		return result * InputLenght;
	}
	else {
		return motionParams.flyingSpeed * InputLenght;
	}
}
FMotionParams UPhysicMotion::GetMotionParams()
{
	float mass = Body->GetMass();
	mass -= initMass;
	mass *= maxMassLerp;
	mass = FMath::Clamp(mass, 0, 1);
	mass = 1 - mass;
	struct FMotionParams result;
	result.forwardSpeed = FMath::Lerp(minMotionParams.forwardSpeed, maxMotionParams.forwardSpeed, mass);
	result.horizontalSpeed = FMath::Lerp(minMotionParams.horizontalSpeed, maxMotionParams.horizontalSpeed, mass);
	result.backSpeed = FMath::Lerp(minMotionParams.backSpeed, maxMotionParams.backSpeed, mass);
	result.acceleration = FMath::Lerp(minMotionParams.acceleration, maxMotionParams.acceleration, mass);
	result.decceleration = FMath::Lerp(minMotionParams.decceleration, maxMotionParams.decceleration, mass);
	result.jumpForce = FMath::Lerp(minMotionParams.jumpForce, maxMotionParams.jumpForce, mass);
	result.flyingSpeed = FMath::Lerp(minMotionParams.flyingSpeed, maxMotionParams.flyingSpeed, mass);
	result.flyAcceleration = FMath::Lerp(minMotionParams.flyAcceleration, maxMotionParams.flyAcceleration, mass);
	result.rotationSpeed = FMath::Lerp(minMotionParams.rotationSpeed, maxMotionParams.rotationSpeed, mass);

	return result;
}
FVector UPhysicMotion::GetForce(FVector WorldDirection, float desiredSpeed, bool isOnTheFloor) {
	FMotionParams motionParams = GetMotionParams();
	FVector Velocity = Body->GetPhysicsLinearVelocity();
	FVector upVector = WorldDirection.Cross(FVector::RightVector);
	upVector.Normalize();
	Velocity = FVector::VectorPlaneProject(Velocity, upVector);
	float mass = Body->GetMass();
	if (isOnTheFloor) {

		FVector ProjectedWorldDirection = FVector::VectorPlaneProject(WorldDirection, FVector::UpVector);
		float maxAceleration = FVector::DotProduct(WorldDirection, Velocity);
		maxAceleration = FMath::Clamp(maxAceleration, 0, 10000000000);
		maxAceleration = desiredSpeed - maxAceleration;
		maxAceleration = FMath::Clamp(maxAceleration, 0, 10000000000);
		maxAceleration /= GetWorld()->GetDeltaSeconds();
		float acceleration = FMath::Clamp(motionParams.acceleration, 0, maxAceleration);
		acceleration *= mass;
		float angle = UMyMaths::Angle(Velocity, ProjectedWorldDirection);
		if (angle > 1) {
			FVector CorrectDirectionForce = WorldDirection.Cross(upVector);
			float correctSpeed = CorrectDirectionForce.Dot(Velocity);
			float maxDeceleration = FMath::Abs(correctSpeed) / GetWorld()->GetDeltaSeconds();
			float deceleration = FMath::Clamp(motionParams.decceleration, 0, maxDeceleration) * mass;
			CorrectDirectionForce.Normalize();
			CorrectDirectionForce *= deceleration * FMath::Sign(correctSpeed);
			FVector Force = WorldDirection * acceleration - CorrectDirectionForce;
			return Force;
		}
		else {
			FVector Force = WorldDirection * acceleration;
			return Force;
		}
	}
	else {
		float currentSpeed = WorldDirection.Dot(Velocity);
		float maxFlyAcceleration = FMath::Clamp(motionParams.flyingSpeed - currentSpeed, 0, 1000000000000);
		maxFlyAcceleration /= GetWorld()->GetDeltaSeconds();
		float flyAcceleration = FMath::Clamp(motionParams.flyAcceleration, 0, maxFlyAcceleration) * mass;
		FVector Force = WorldDirection * flyAcceleration;
		return Force;
	}
}
void UPhysicMotion::AddJumpImpulse(FVector XYWorldDirection) {
	float lerp = XYWorldDirection.Length();
	lerp /= 1.4142f;
	lerp /= JumpXYLerp;
	float upLenght = FMath::Lerp(1.0f, 0.0f, lerp);
	FVector upForce = FVector::UpVector * upLenght;
	XYWorldDirection.Normalize();
	XYWorldDirection.Z = 0;
	FVector HorizontalForce = XYWorldDirection * HorizontalJumpForcePercent;
	FVector JumpForce = upForce + HorizontalForce;
	FMotionParams motionParams = GetMotionParams();
	//float mass = initMass / Body->GetMass();
	//float mass = initMass;
	float mass = Body->GetMass();
	JumpForce = JumpForce * motionParams.jumpForce * mass;
	Body->AddImpulse(JumpForce, "None", false);
}

