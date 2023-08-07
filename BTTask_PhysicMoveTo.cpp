// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Possessed/BTTask_PhysicMoveTo.h"
#include "Characters/Possessed/AIPhysicMoveTo.h"
#include <DiablosYares/Characters/Possessed/DYBasePossessed.h>
#include <BehaviorTree/BlackboardComponent.h>
#include <AIController.h>
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
UBTTask_PhysicMoveTo::UBTTask_PhysicMoveTo()
{
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTTask_PhysicMoveTo::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	BehaviorTreeComponent = &OwnerComp;
	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	AAIController* myController = OwnerComp.GetAIOwner();
	myPossessed = myController ? Cast<ADYBasePossessed>(myController->GetPawn()) : nullptr;
	if (!BlackboardComponent || !myPossessed)
		return EBTNodeResult::Failed;
	const UBlackboardComponent* MyBlackboard = OwnerComp.GetBlackboardComponent();
	
	if (BlackboardKey.SelectedKeyType == UBlackboardKeyType_Object::StaticClass()) {
		UObject* KeyValue = BlackboardComponent->GetValue<UBlackboardKeyType_Object>(BlackboardKey.GetSelectedKeyID());
		AActor* TargetActor = Cast<AActor>(KeyValue);
		if (TargetActor) {
			myPossessed->PhysicFollowActor(TargetActor);
			myPossessed->AIPhysicMoveTo->Enable();
			myPossessed->AIPhysicMoveTo->OnReachTargetLocation.AddDynamic(this, &UBTTask_PhysicMoveTo::ReachTargetLocation);
			return EBTNodeResult::InProgress;
		}
	}else if (BlackboardKey.SelectedKeyType == UBlackboardKeyType_Vector::StaticClass()) {

		if (BlackboardComponent->IsVectorValueSet(BlackboardKey.GetSelectedKeyID())) {
			const FVector TargetLocation = BlackboardComponent->GetValue<UBlackboardKeyType_Vector>(BlackboardKey.GetSelectedKeyID());
			myPossessed->PhysicMoveLocation(TargetLocation);
			myPossessed->AIPhysicMoveTo->Enable();
			myPossessed->AIPhysicMoveTo->OnReachTargetLocation.AddDynamic(this, &UBTTask_PhysicMoveTo::ReachTargetLocation);
			return EBTNodeResult::InProgress;
		}
	}
	
	return EBTNodeResult::Aborted;
}
void UBTTask_PhysicMoveTo::ReachTargetLocation() {
	myPossessed->AIPhysicMoveTo->OnReachTargetLocation.RemoveDynamic(this, &UBTTask_PhysicMoveTo::ReachTargetLocation);
	myPossessed->AIPhysicMoveTo->Disable();
	//BehaviorTreeComponent->OnTaskFinished(this, EBTNodeResult::Succeeded);
	FinishLatentTask(*BehaviorTreeComponent, EBTNodeResult::Succeeded);
}
EBTNodeResult::Type UBTTask_PhysicMoveTo::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	myPossessed->AIPhysicMoveTo->OnReachTargetLocation.RemoveDynamic(this, &UBTTask_PhysicMoveTo::ReachTargetLocation);
	myPossessed->AIPhysicMoveTo->Disable();
	return Super::AbortTask(OwnerComp, NodeMemory);
}

