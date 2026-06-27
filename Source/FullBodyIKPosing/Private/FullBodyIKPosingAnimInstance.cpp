// Fill out your copyright notice in the Description page of Project Settings.


#include "FullBodyIKPosingAnimInstance.h"

void UFullBodyIKPosingAnimInstance::RegisterSnapshotNode(FName InSnapshotName,
	FAnimNode_SaveBoneTransformSnapshot* InNode)
{
	if (InSnapshotName != NAME_None && InNode != nullptr)
	{
		RegisteredSnapshotNodes.Add(InSnapshotName, InNode);
	}
}

bool UFullBodyIKPosingAnimInstance::GetSnapshotBoneTransform(FName SnapshotName, FName BoneName,
	FTransform& OutTransform) const
{
	if (FAnimNode_SaveBoneTransformSnapshot* const* Found = RegisteredSnapshotNodes.Find(SnapshotName))
	{
		return (*Found)->GetBoneTransform(BoneName, OutTransform);
	}
	return false;
}

bool UFullBodyIKPosingAnimInstance::GetAllSnapshotBoneTransforms(FName SnapshotName,
	TMap<FName, FTransform>& OutTransforms) const
{
	if (FAnimNode_SaveBoneTransformSnapshot* const* Found = RegisteredSnapshotNodes.Find(SnapshotName))
	{
		(*Found)->GetAllBoneTransforms(OutTransforms);
		return true;
	}
	return false;
}
