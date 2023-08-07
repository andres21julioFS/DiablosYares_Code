// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_PhysicMoveTo.generated.h"

class ADYBasePossessed;

class UBehaviorTreeComponent;
UCLASS()
class DIABLOSYARES_API UBTTask_PhysicMoveTo : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	UFUNCTION()
	void ReachTargetLocation();
	UBehaviorTreeComponent* BehaviorTreeComponent;
	ADYBasePossessed* myPossessed;
public:
	UBTTask_PhysicMoveTo();
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
