// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Grabber.generated.h"

class UPhysicsHandleComponent;
class UPrimitiveComponent;

USTRUCT(BlueprintType)
struct FGrabberParams
{
	GENERATED_BODY()
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float heightBodyLineTrace;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float grabDistance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float maxGrabDistance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float grabHeight;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		FName grabableTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float GrabMassPercent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float RotationSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float HorizontalRotationSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float DistanceSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float MinGrabZLocation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float MaxGrabZLocation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float SpeedGrabZLocation;
	FGrabberParams()
	{
		heightBodyLineTrace = 0;
		grabDistance = 50;
		maxGrabDistance = 150;
		HorizontalRotationSpeed = 200;
		grabHeight = 20;
		grabableTag = TEXT("grabable");
		GrabMassPercent = 1;
		RotationSpeed = 10;
		DistanceSpeed = 5;
		MinGrabZLocation = -50;
		MaxGrabZLocation = 50;
		SpeedGrabZLocation = 100;
	}
};

UCLASS(ClassGroup = (Custom), BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent))
class DIABLOSYARES_API UGrabber : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UGrabber();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	float GetDistanceToBody(FVector& Position);
	void GetGrabLocationAndRotation(FVector TargetWorldLocation, const FRotator& TargetWorldRotation, FVector& Position, FRotator& Rotation);
public:
	bool IsGrabbing;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void SetComponents(UPhysicsHandleComponent* PhysicsHandle, UPrimitiveComponent* Body, float _BodyHalfHeight);
	bool AttemptGrab(FVector WorldLocation, FRotator WorldRotation, UPrimitiveComponent* PrimitiveComponent);
	void ReleaseGrabbedObject();
	bool HoldGrabbedObject(FVector WorldLocation, FRotator WorldRotation);
	void SetZGrabLocation(float value);
	bool CheckObjectIsGrabbable(UPrimitiveComponent* Object, FVector WorldLocation);
	void HighlightGrabbedObject(bool value);
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		FGrabberParams GrabberParams;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		float maxMotionParams;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		float BodyHalfHeight;
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		float ZGrabLocation;
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		float CurrentRodeoSpeed;
	float Object_GrabLocationDistance;
	UPhysicsHandleComponent* PhysicsHandle;
	UPrimitiveComponent* GrabbedObject;
	UPrimitiveComponent* Body;
	UPrimitiveComponent* LastPrimitiveComponent;
	ECollisionResponse GrabbedObjectCollisionResponse;
};
