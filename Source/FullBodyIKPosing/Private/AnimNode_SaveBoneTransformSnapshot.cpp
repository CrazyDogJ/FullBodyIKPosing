// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNode_SaveBoneTransformSnapshot.h"

#include "FullBodyIKPosingAnimInstance.h"
#include "Animation/AnimInstanceProxy.h"

FAnimNode_SaveBoneTransformSnapshot::FAnimNode_SaveBoneTransformSnapshot()
	: SnapshotLock(MakeShared<FCriticalSection, ESPMode::ThreadSafe>())
{
}

void FAnimNode_SaveBoneTransformSnapshot::Initialize_AnyThread(const FAnimationInitializeContext& Context)
{
	BasePose.Initialize(Context);

	// 把自己注册到AnimInstance上,这样外部蓝图调用AnimInstance的接口时
	// 才能按SnapshotName找到对应的节点实例
	if (UFullBodyIKPosingAnimInstance* AnimInst = Cast<UFullBodyIKPosingAnimInstance>(Context.AnimInstanceProxy->GetAnimInstanceObject()))
	{
		AnimInst->RegisterSnapshotNode(SnapshotName, this);
	}
}

void FAnimNode_SaveBoneTransformSnapshot::CacheBones_AnyThread(const FAnimationCacheBonesContext& Context)
{
	BasePose.CacheBones(Context);

	// FBoneReference必须在这里Initialize一次,缓存对应的CompactPoseIndex
	for (FBoneReference& BoneRef : BonesToSave)
	{
		BoneRef.Initialize(Context.AnimInstanceProxy->GetRequiredBones());
	}
}

void FAnimNode_SaveBoneTransformSnapshot::Update_AnyThread(const FAnimationUpdateContext& Context)
{
	BasePose.Update(Context);
}

void FAnimNode_SaveBoneTransformSnapshot::Evaluate_AnyThread(FPoseContext& Output)
{
	BasePose.Evaluate(Output);

	const FBoneContainer& BoneContainer = Output.Pose.GetBoneContainer();
	TMap<FName, FTransform> SnapshotMap;

	if (bSaveInComponentSpace)
	{
		FCSPose<FCompactPose> ComponentSpacePose;
		ComponentSpacePose.InitPose(Output.Pose);

		if (BonesToSave.Num() > 0)
		{
			for (const FBoneReference& BoneRef : BonesToSave)
			{
				const FCompactPoseBoneIndex CompactIndex = BoneRef.GetCompactPoseIndex(BoneContainer);
				if (CompactIndex != INDEX_NONE)
				{
					SnapshotMap.Add(BoneRef.BoneName, ComponentSpacePose.GetComponentSpaceTransform(CompactIndex));
				}
			}
		}
		else
		{
			for (FCompactPoseBoneIndex BoneIndex(0); BoneIndex < Output.Pose.GetNumBones(); ++BoneIndex)
			{
				const FMeshPoseBoneIndex MeshIndex = BoneContainer.MakeMeshPoseIndex(BoneIndex);
				const FName BoneName = BoneContainer.GetReferenceSkeleton().GetBoneName(MeshIndex.GetInt());
				SnapshotMap.Add(BoneName, ComponentSpacePose.GetComponentSpaceTransform(BoneIndex));
			}
		}
	}
	else
	{
		if (BonesToSave.Num() > 0)
		{
			for (const FBoneReference& BoneRef : BonesToSave)
			{
				const FCompactPoseBoneIndex CompactIndex = BoneRef.GetCompactPoseIndex(BoneContainer);
				if (CompactIndex != INDEX_NONE)
				{
					SnapshotMap.Add(BoneRef.BoneName, Output.Pose[CompactIndex]);
				}
			}
		}
		else
		{
			for (FCompactPoseBoneIndex BoneIndex(0); BoneIndex < Output.Pose.GetNumBones(); ++BoneIndex)
			{
				const FMeshPoseBoneIndex MeshIndex = BoneContainer.MakeMeshPoseIndex(BoneIndex);
				const FName BoneName = BoneContainer.GetReferenceSkeleton().GetBoneName(MeshIndex.GetInt());
				SnapshotMap.Add(BoneName, Output.Pose[BoneIndex]);
			}
		}
	}

	// 加锁写入,游戏线程随时可能在读
	{
		FScopeLock Lock(SnapshotLock.Get());
		LatestSnapshot = MoveTemp(SnapshotMap);
	}
}

void FAnimNode_SaveBoneTransformSnapshot::GatherDebugData(FNodeDebugData& DebugData)
{
	DebugData.AddDebugItem(DebugData.GetNodeName(this));
	BasePose.GatherDebugData(DebugData);
}

bool FAnimNode_SaveBoneTransformSnapshot::GetBoneTransform(FName BoneName, FTransform& OutTransform) const
{
	FScopeLock Lock(SnapshotLock.Get());
	if (const FTransform* Found = LatestSnapshot.Find(BoneName))
	{
		OutTransform = *Found;
		return true;
	}
	return false;
}

void FAnimNode_SaveBoneTransformSnapshot::GetAllBoneTransforms(TMap<FName, FTransform>& OutTransforms) const
{
	FScopeLock Lock(SnapshotLock.Get());
	OutTransforms = LatestSnapshot;
}
