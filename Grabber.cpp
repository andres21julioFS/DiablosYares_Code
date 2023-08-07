// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Grabber.h"
#include "Components/PrimitiveComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "Engine/World.h"
#include "Kismet/KismetMathLibrary.h"
#include "FunctionsLibrary/MyMaths.h"
#include "DrawDebugHelpers.h"
// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();

	// ...

}


// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}
void UGrabber::SetComponents(UPhysicsHandleComponent* _PhysicsHandle, UPrimitiveComponent* _Body, float _BodyHalfHeight) {
	PhysicsHandle = _PhysicsHandle;
	Body = _Body;
	BodyHalfHeight = _BodyHalfHeight;
}
bool UGrabber::AttemptGrab(FVector WorldLocation, FRotator WorldRotation, UPrimitiveComponent* PrimitiveComponent) {
	FHitResult OutHit;
	FVector BodyLocation = Body->GetComponentLocation();
	FVector Start = BodyLocation + FVector::UpVector * GrabberParams.heightBodyLineTrace;
	FVector Dir = WorldLocation - Start;
	FVector End = Start + Dir * 2;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Body->GetOwner());
	FCollisionResponseParams ResponseParam;
	bool thereIsHit = GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_Visibility, Params, ResponseParam);
	if (thereIsHit && OutHit.GetActor() == PrimitiveComponent->GetOwner()) {
		if (IsGrabbing) return false;
		if (PrimitiveComponent->Mobility == EComponentMobility::Static)return false;
		if (!PrimitiveComponent->GetOwner()->ActorHasTag(GrabberParams.grabableTag) && !PrimitiveComponent->ComponentHasTag(GrabberParams.grabableTag)) {
			return false;
		}
		float distance = GetDistanceToBody(OutHit.ImpactPoint);
		if (distance > GrabberParams.maxGrabDistance)return false;
		ZGrabLocation = WorldLocation.Z - BodyLocation.Z;
		GrabbedObject = PrimitiveComponent;
		Object_GrabLocationDistance = FVector::Distance(GrabbedObject->GetComponentLocation(), WorldLocation);
		GrabbedObjectCollisionResponse = GrabbedObject->GetCollisionResponseToChannel(ECollisionChannel::ECC_Camera);
		GrabbedObject->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
		PhysicsHandle->GrabComponentAtLocationWithRotation(GrabbedObject, "None", WorldLocation, WorldRotation);
		IsGrabbing = true;
		return true;
	}
	else {
		return false;
	}
}
void UGrabber::ReleaseGrabbedObject() {
	if (IsGrabbing) {
		IsGrabbing = false;
		GrabbedObject->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, GrabbedObjectCollisionResponse);
		PhysicsHandle->ReleaseComponent();
	}
}
bool UGrabber::HoldGrabbedObject(FVector WorldLocation, FRotator WorldRotation) {
	if (IsGrabbing) {
		FVector Location;
		FRotator Rotation;
		PhysicsHandle->GetTargetLocationAndRotation(Location, Rotation);
		FVector GrabbedObjectLocation = GrabbedObject->GetComponentLocation();
		float distance = GetDistanceToBody(GrabbedObjectLocation) - Object_GrabLocationDistance;
		if (distance <= GrabberParams.maxGrabDistance) {
			FVector NewPosition;
			FRotator NewRotation;
			GetGrabLocationAndRotation(WorldLocation, WorldRotation, NewPosition, NewRotation);
			PhysicsHandle->SetTargetLocationAndRotation(NewPosition, NewRotation);
			return true;
		}
		else {
			ReleaseGrabbedObject();
			return false;
		}
	}
	else {
		return false;
	}
}

void UGrabber::GetGrabLocationAndRotation(FVector TargetWorldLocation, const FRotator& TargetWorldRotation, FVector& Position, FRotator& Rotation) {

	FVector CurrentLocation;
	FRotator CurrentRotator;
	PhysicsHandle->GetTargetLocationAndRotation(CurrentLocation, CurrentRotator);
	float BodyZPos = Body->GetComponentLocation().Z;
	FVector Z0CurrentLocation = CurrentLocation;
	Z0CurrentLocation.Z = 0;
	float ZPos= FMath::Clamp(BodyZPos + ZGrabLocation, GrabberParams.MinGrabZLocation+ BodyZPos ,  GrabberParams.MaxGrabZLocation + BodyZPos);
	TargetWorldLocation.Z = ZPos;
	FVector dir1 = CurrentLocation - Body->GetComponentLocation();
	
	FVector dir2 = TargetWorldLocation - Body->GetComponentLocation();
	FVector dirNewRotation = dir1;
	dir1.Z = 0;
	dir2.Z = 0;
	FVector UpAxis = FVector::CrossProduct(dir1, dir2);
	UpAxis.Normalize();
	//float maxAngle1 = UMyMaths::Angle(dir1, dir2);
	float maxAngle1 = UMyMaths::Angle(dir1, dir2);
	FVector verticalDir3 = dir1.RotateAngleAxis(maxAngle1, UpAxis);
	float maxAngle3 = UMyMaths::Angle(dir2, verticalDir3);
	FVector RightAxis = FVector::CrossProduct(verticalDir3,dir2);
	RightAxis.Normalize();
	
	
	float angle1 = FMath::Clamp(GrabberParams.HorizontalRotationSpeed * GetWorld()->GetDeltaSeconds(), 0, maxAngle1);
	FVector NewDir = dir1.RotateAngleAxis(angle1, UpAxis);
	dirNewRotation = dirNewRotation.RotateAngleAxis(angle1, UpAxis);
	
	NewDir.Normalize();
	FVector Pos1 = TargetWorldLocation;
	FVector Pos2 = Body->GetComponentLocation();
	Pos1.Z = 0;
	Pos2.Z = 0;
	float distance = FVector::Distance(Pos1, Pos2);
	float difference = GrabberParams.grabDistance - distance;
	float sign = FMath::Sign(difference);
	distance += FMath::Clamp(GrabberParams.DistanceSpeed, 0, FMath::Abs(difference)) * sign;
	NewDir *= distance;
	FVector Z0BodyPos = Body->GetComponentLocation();
	Z0BodyPos.Z = 0;
	Position = NewDir + Z0BodyPos + FVector::UpVector* ZPos;
	
	//NewDir.Z = 0;
	float alpha = GrabberParams.RotationSpeed*GetWorld()->GetDeltaSeconds();
	//FVector NewRotation = CurrentRotator.Vector().RotateAngleAxis(angle1, UpAxis);
	FVector NewRotation = CurrentRotator.Quaternion()*NewDir;
	Rotation = FQuat::Slerp(CurrentRotator.Quaternion(), NewDir.Rotation().Quaternion(), alpha).Rotator();
}
float UGrabber::GetDistanceToBody(FVector& Position) {
	FVector Start = Body->GetComponentLocation() - FVector::UpVector * BodyHalfHeight;
	FVector End = Body->GetComponentLocation() + FVector::UpVector * BodyHalfHeight;
	return UKismetMathLibrary::GetPointDistanceToSegment(Position, Start, End);
}
void UGrabber::SetZGrabLocation(float value) {
	if (IsGrabbing) {
		ZGrabLocation = FMath::Clamp(ZGrabLocation + value * GrabberParams.SpeedGrabZLocation * GetWorld()->GetDeltaSeconds(), GrabberParams.MinGrabZLocation, GrabberParams.MaxGrabZLocation);
	}
}
bool UGrabber::CheckObjectIsGrabbable(UPrimitiveComponent* PrimitiveComponent, FVector WorldLocation) {

	FHitResult OutHit;
	FVector Start = Body->GetComponentLocation() + FVector::UpVector * GrabberParams.heightBodyLineTrace;
	FVector Dir = WorldLocation - Start;
	FVector End = Start + Dir * 2;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Body->GetOwner());
	FCollisionResponseParams ResponseParam;

	if (LastPrimitiveComponent) {
		HighlightGrabbedObject(false);
	}
	if (PrimitiveComponent) {
		bool thereIsHit = GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_Visibility, Params, ResponseParam);
		if (thereIsHit && OutHit.GetActor() == PrimitiveComponent->GetOwner()) {
			if (PrimitiveComponent->Mobility == EComponentMobility::Static)return false;
			if (!PrimitiveComponent->GetOwner()->ActorHasTag(GrabberParams.grabableTag) && !PrimitiveComponent->ComponentHasTag(GrabberParams.grabableTag)) {
				return false;
			}
			float distance = GetDistanceToBody(OutHit.ImpactPoint);
			if (distance > GrabberParams.maxGrabDistance)return false;

			HighlightGrabbedObject(true);
			LastPrimitiveComponent = PrimitiveComponent;
			return true;
		}
	}
	return false;
}
void UGrabber::HighlightGrabbedObject(bool value) {
	if (LastPrimitiveComponent) {
		LastPrimitiveComponent->SetRenderCustomDepth(value);
	}
}

