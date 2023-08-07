// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/PrimitiveComponent.h"
#include "PhysicMotion.generated.h"

class UPrimitiveComponent;
USTRUCT(BlueprintType)
struct FMotionParams
{
	GENERATED_BODY()
		//GENERATED_USTRUCT_BODY()
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float forwardSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float backSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float horizontalSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float acceleration;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float decceleration;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float jumpForce;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float flyingSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float flyAcceleration;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float rotationSpeed;
	FMotionParams()
	{
		forwardSpeed = 600;
		backSpeed = 400;
		horizontalSpeed = 300;
		acceleration = 2500;
		decceleration = 2000;
		jumpForce = 400;
		flyingSpeed = 200;
		flyAcceleration = 700;
		rotationSpeed = 500;
	}
};
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DIABLOSYARES_API UPhysicMotion : public UActorComponent
{
	GENERATED_BODY()
		UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		FMotionParams maxMotionParams;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		FMotionParams minMotionParams;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		float maxMassLerp = 100;

public:
	// Sets default values for this component's properties
	UPhysicMotion();
	void AddJumpImpulse(FVector XYWorldDirection);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		float initMass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		UPrimitiveComponent* Body;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		USceneComponent* RotationBody;
	FMotionParams GetMotionParams();
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	void SetPrimitiveComponent(UPrimitiveComponent* PrimitiveComponent, USceneComponent* RotationComponent, float _offsetRotationAngle);
	void UpdateMotion(float NormalizedSpeed, FVector WorldDirection, FVector LookDirection, bool IsOnTheFloor, bool CanMove, bool CanImmovilize);
	void LookToDirection(FVector WorldDirection);
private:

	void Immovilize(bool IsOnTheFloor);
	float GetMaxSpeed(float InputLenght, float angle, bool isOnTheFloor);
	FVector GetForce(FVector WorldDirection, float desiredSpeed, bool isOnTheFloor);

	float JumpXYLerp = 2.5f;
	float HorizontalJumpForcePercent = 0.25f;
	float offsetRotationAngle;
};
