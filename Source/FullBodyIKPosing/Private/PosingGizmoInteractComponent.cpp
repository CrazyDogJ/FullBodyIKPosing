// Fill out your copyright notice in the Description page of Project Settings.


#include "PosingGizmoInteractComponent.h"

#include "PosingGizmoComponent.h"
#include "Camera/CameraModifier.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

UPosingGizmoInteractComponent::UPosingGizmoInteractComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UPosingGizmoInteractComponent::ApplyCameraModifier()
{
	const auto PC = Cast<APlayerController>(GetOwner());
	if (PC)
	{
		OwningPlayerController = PC;
		if (CameraModifier)
		{
			OwningPlayerController->PlayerCameraManager->AddNewCameraModifier(CameraModifier);
		}
	}
}

void UPosingGizmoInteractComponent::BeginPlay()
{
	Super::BeginPlay();

	ApplyCameraModifier();
}

void UPosingGizmoInteractComponent::DraggingGizmo() const
{
	if (OwningPlayerController)
	{
		float X;
		float Y;
		OwningPlayerController->GetMousePosition(X, Y);

		FVector WorldPos;
		FVector WorldDir;
		UGameplayStatics::DeprojectScreenToWorld(OwningPlayerController, FVector2D(X, Y), WorldPos, WorldDir);

		if (DraggingPosingGizmo)
		{
			const auto CompWorldLoc = WorldPos + WorldDir * ImpactPointDistance - OffsetLocation;
			DraggingPosingGizmo->SetWorldLocation(CompWorldLoc);
		}
	}
}

void UPosingGizmoInteractComponent::UpdatePawnCanControl() const
{
	if (OwningPlayerController)
	{
		const auto Pawn = OwningPlayerController->GetPawn();
		if (Pawn)
		{
			if (DraggingPosingGizmo)
			{
				Pawn->DisableInput(OwningPlayerController);
			}
			else
			{
				Pawn->EnableInput(OwningPlayerController);
			}
		}
	}
}

void UPosingGizmoInteractComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
                                                  FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	DraggingGizmo();
	UpdatePawnCanControl();
}

void UPosingGizmoInteractComponent::UpdateGizmoSelectedEffect(bool bSelected)
{
	if (DraggingPosingGizmo)
	{
		DraggingPosingGizmo->UpdateCustomStencil(bSelected);
	}
}

void UPosingGizmoInteractComponent::MouseTrace( FVector& Start, FVector& End) const
{
	if (OwningPlayerController)
	{
		float X;
		float Y;
		OwningPlayerController->GetMousePosition(X, Y);

		FVector WorldPos;
		FVector WorldDir;
		UGameplayStatics::DeprojectScreenToWorld(OwningPlayerController, FVector2D(X, Y), WorldPos, WorldDir);

		Start = WorldPos;
		End = Start + WorldDir * 100000.0f;
	}
}

void UPosingGizmoInteractComponent::LineTraceAndSetup()
{
	FVector Start, End;
	MouseTrace(Start, End);

	FHitResult Hit;
	GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECollisionChannel::ECC_Visibility);

	const auto NewHitGizmo = Cast<UPosingGizmoComponent>(Hit.GetComponent());
	if (!NewHitGizmo)
	{
		return;
	}

	if (DraggingPosingGizmo && DraggingPosingGizmo != NewHitGizmo)
	{
		DraggingPosingGizmo->bIsDragging = false;
	}

	DraggingPosingGizmo = NewHitGizmo;
	DraggingPosingGizmo->bIsDragging = true;
	
	OffsetLocation = Hit.ImpactPoint - DraggingPosingGizmo->GetComponentLocation();
	
	if (OwningPlayerController)
	{
		float X;
		float Y;
		OwningPlayerController->GetMousePosition(X, Y);
		FVector WorldPos;
		FVector WorldDir;
		UGameplayStatics::DeprojectScreenToWorld(OwningPlayerController, FVector2D(X, Y), WorldPos, WorldDir);
		ImpactPointDistance = FVector::Dist(WorldPos, Hit.ImpactPoint);
	}
}

void UPosingGizmoInteractComponent::ClearDraggingGizmo()
{
	if (DraggingPosingGizmo)
	{
		DraggingPosingGizmo->bIsDragging = false;
		DraggingPosingGizmo = nullptr;
	}
}

FRotator UPosingGizmoInteractComponent::CalculateGizmoRotation(const FVector2D& RotationDir) const
{
	if (OwningPlayerController && OwningPlayerController->PlayerCameraManager)
	{
		const float YAngle = RotationSpeed * GetWorld()->GetDeltaSeconds() * RotationDir.Y;
		const float XAngle = RotationSpeed * GetWorld()->GetDeltaSeconds() * RotationDir.X * -1;

		const auto CamRot = OwningPlayerController->PlayerCameraManager->GetCameraRotation();
		const auto ARot = UKismetMathLibrary::RotatorFromAxisAndAngle(UKismetMathLibrary::GetRightVector(CamRot), YAngle);
		const auto BRot = UKismetMathLibrary::RotatorFromAxisAndAngle(UKismetMathLibrary::GetUpVector(CamRot), XAngle);
		return UKismetMathLibrary::ComposeRotators(ARot, BRot);
	}

	return FRotator();
}

void UPosingGizmoInteractComponent::ClickEvent()
{
	if (OwningPlayerController)
	{
		UpdateGizmoSelectedEffect(false);
		LineTraceAndSetup();
		UpdateGizmoSelectedEffect(true);
	}
}

void UPosingGizmoInteractComponent::ReleaseEvent()
{
	UpdateGizmoSelectedEffect(false);
	ClearDraggingGizmo();
}

void UPosingGizmoInteractComponent::RotationEvent(const FVector2D InputVector) const
{
	if (DraggingPosingGizmo)
	{
		const auto Rotator = CalculateGizmoRotation(InputVector);
		DraggingPosingGizmo->AddWorldRotation(Rotator);
	}
}
