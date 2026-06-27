// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PosingGizmoInteractComponent.generated.h"

class UCameraModifier;
class UPosingGizmoComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FULLBODYIKPOSING_API UPosingGizmoInteractComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPosingGizmoInteractComponent();

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	UPosingGizmoComponent* DraggingPosingGizmo = nullptr;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	APlayerController* OwningPlayerController = nullptr;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<UCameraModifier> CameraModifier;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	float ImpactPointDistance = 0.0f;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	FVector OffsetLocation = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float RotationSpeed = 90.0f;

protected:
	void ApplyCameraModifier();
	virtual void BeginPlay() override;
	void DraggingGizmo() const;
	void UpdatePawnCanControl() const;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void UpdateGizmoSelectedEffect(bool bSelected);
	void MouseTrace(FVector& Start, FVector& End) const;
	void LineTraceAndSetup();
	void ClearDraggingGizmo();
	FRotator CalculateGizmoRotation(const FVector2D& RotationDir) const;
	
public:
	// Inputs

	UFUNCTION(BlueprintCallable)
	void ClickEvent();

	UFUNCTION(BlueprintCallable)
	void ReleaseEvent();

	UFUNCTION(BlueprintCallable)
	void RotationEvent(const FVector2D InputVector) const;
};
