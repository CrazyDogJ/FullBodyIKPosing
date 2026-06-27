// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimGraphNode_SaveBoneTransformSnapshot.h"

FText UAnimGraphNode_SaveBoneTransformSnapshot::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return Node.SnapshotName != NAME_None
		? FText::Format(NSLOCTEXT("SaveBoneSnapshot", "Title", "保存骨骼快照: {0}"), FText::FromName(Node.SnapshotName))
		: NSLOCTEXT("SaveBoneSnapshot", "TitleDefault", "保存骨骼快照");
}

FText UAnimGraphNode_SaveBoneTransformSnapshot::GetTooltipText() const
{
	return NSLOCTEXT("SaveBoneSnapshot", "Tooltip", "旁路保存当前姿势,不修改姿势本身,可在其他蓝图通过AnimInstance查询骨骼变换。");
}

FLinearColor UAnimGraphNode_SaveBoneTransformSnapshot::GetNodeTitleColor() const
{
	return FLinearColor(0.2f, 0.6f, 1.0f);
}

FString UAnimGraphNode_SaveBoneTransformSnapshot::GetNodeCategory() const
{
	return TEXT("Utility");
}
