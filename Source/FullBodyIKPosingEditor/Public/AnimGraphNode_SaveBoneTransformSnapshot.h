// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnimNode_SaveBoneTransformSnapshot.h"
#include "AnimGraph/Classes/AnimGraphNode_Base.h"
#include "AnimGraphNode_SaveBoneTransformSnapshot.generated.h"

/**
 * 
 */
UCLASS()
class FULLBODYIKPOSINGEDITOR_API UAnimGraphNode_SaveBoneTransformSnapshot : public UAnimGraphNode_Base
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = Settings)
	FAnimNode_SaveBoneTransformSnapshot Node;

	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FText GetTooltipText() const override;
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual FString GetNodeCategory() const override;
};
