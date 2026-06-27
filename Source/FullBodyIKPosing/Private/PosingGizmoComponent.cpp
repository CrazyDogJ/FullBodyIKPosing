// Fill out your copyright notice in the Description page of Project Settings.


#include "PosingGizmoComponent.h"

#include "FullBodyIKPosingAnimInstance.h"
#include "Kismet/KismetMathLibrary.h"

UPosingGizmoComponent::UPosingGizmoComponent()
	: Super()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UPosingGizmoComponent::ClearOffset()
{
	OffsetTransform = FTransform();
}

void UPosingGizmoComponent::UpdateCustomStencil(const bool& bSelected)
{
	if (bSelected)
	{
		SetCustomDepthStencilValue(SelectCustomStencil);
	}
	else
	{
		SetCustomDepthStencilValue(DefaultCustomStencil);
	}
}

void UPosingGizmoComponent::UpdateVisible()
{
	SetVisibility(bIsGizmoVisible);
	SetCollisionEnabled(bIsGizmoVisible ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
	SetRenderCustomDepth(bIsGizmoVisible);
}

FTransform UPosingGizmoComponent::GetOffsetTransform() const
{
	const auto Relative = GetRelativeTransform();
	
	return UKismetMathLibrary::MakeRelativeTransform(
		FTransform(Relative.GetRotation(), Relative.GetLocation()),
		FTransform(InitTransform.GetRotation(), InitTransform.GetLocation()));
}

void UPosingGizmoComponent::UpdateInitTransform()
{
	const auto ParentSkelMesh = Cast<USkeletalMeshComponent>(GetAttachParent());
	if (!ParentSkelMesh)
	{
		return;
	}
	
	const auto AnimInstance = Cast<UFullBodyIKPosingAnimInstance>(ParentSkelMesh->GetAnimInstance());
	if (!AnimInstance)
	{
		return;
	}

	FTransform OutTransform;
	if (AnimInstance->GetSnapshotBoneTransform(SnapshotName, BoneName, OutTransform))
	{
		InitTransform = OutTransform;
	}

	if (!bIsDragging)
	{
		SetRelativeTransform(FTransform(
			InitTransform.TransformRotation(OffsetTransform.GetRotation()),
			InitTransform.TransformPosition(OffsetTransform.GetLocation()),
			GizmoScale
		));
	}
}

void UPosingGizmoComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateVisible();
	
	if (bIsDragging)
	{
		OffsetTransform = GetOffsetTransform();
	}

	UpdateInitTransform();
}
