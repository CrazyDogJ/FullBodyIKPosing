// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "AnimNode_SaveBoneTransformSnapshot.generated.h"

/**
 * 
 */
USTRUCT(BlueprintInternalUseOnly)
struct FULLBODYIKPOSING_API FAnimNode_SaveBoneTransformSnapshot : public FAnimNode_Base
{
	GENERATED_BODY()

public:
	// 上游姿势输入,本节点原样转发,不做任何修改
	UPROPERTY(EditAnywhere, Category = Links)
	FPoseLink BasePose;

	// 快照标识名,外部蓝图按这个名字查询(同一个AnimBP里可以放多个快照点)
	UPROPERTY(EditAnywhere, Category = Settings)
	FName SnapshotName;

	// 需要保存的骨骼,留空则保存全部骨骼(全骨骼开销较大,建议按需勾选)
	UPROPERTY(EditAnywhere, Category = Settings)
	TArray<FBoneReference> BonesToSave;

	// true = 组件空间(相对SkeletalMeshComponent),false = 本地空间(相对父骨骼)
	UPROPERTY(EditAnywhere, Category = Settings)
	bool bSaveInComponentSpace = true;

public:
	FAnimNode_SaveBoneTransformSnapshot();

	// FAnimNode_Base接口
	virtual void Initialize_AnyThread(const FAnimationInitializeContext& Context) override;
	virtual void CacheBones_AnyThread(const FAnimationCacheBonesContext& Context) override;
	virtual void Update_AnyThread(const FAnimationUpdateContext& Context) override;
	virtual void Evaluate_AnyThread(FPoseContext& Output) override;
	virtual void GatherDebugData(FNodeDebugData& DebugData) override;

	// 供AnimInstance在游戏线程查询(内部加锁,线程安全)
	bool GetBoneTransform(FName BoneName, FTransform& OutTransform) const;
	void GetAllBoneTransforms(TMap<FName, FTransform>& OutTransforms) const;

private:
	// 用SharedPtr包裹,避免节点结构体被蓝图编译器拷贝时,
	// FCriticalSection本身不可拷贝导致的问题
	TSharedPtr<FCriticalSection, ESPMode::ThreadSafe> SnapshotLock;

	// 最近一次Evaluate写入的快照,按骨骼名索引(跨LOD稳定)
	TMap<FName, FTransform> LatestSnapshot;
};
