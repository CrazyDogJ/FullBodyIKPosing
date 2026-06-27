// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnimNode_SaveBoneTransformSnapshot.h"
#include "Animation/AnimInstance.h"
#include "FullBodyIKPosingAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class FULLBODYIKPOSING_API UFullBodyIKPosingAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FTransform> GizmoTransforms;
	
	// 由快照节点在Initialize_AnyThread里调用,完成注册
	void RegisterSnapshotNode(FName InSnapshotName, FAnimNode_SaveBoneTransformSnapshot* InNode);

	// 给角色蓝图/组件蓝图等外部逻辑调用
	UFUNCTION(BlueprintCallable, Category = "Animation|BoneSnapshot")
	bool GetSnapshotBoneTransform(FName SnapshotName, FName BoneName, FTransform& OutTransform) const;

	UFUNCTION(BlueprintCallable, Category = "Animation|BoneSnapshot")
	bool GetAllSnapshotBoneTransforms(FName SnapshotName, TMap<FName, FTransform>& OutTransforms) const;

private:
	// key: SnapshotName, value: 节点指针(节点生命周期与AnimInstance一致)
	TMap<FName, FAnimNode_SaveBoneTransformSnapshot*> RegisteredSnapshotNodes;
};
