// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "PosingGizmoComponent.generated.h"

/**
 * 
 */
UCLASS(meta=(BlueprintSpawnableComponent))
class FULLBODYIKPOSING_API UPosingGizmoComponent : public UStaticMeshComponent
{
	GENERATED_BODY()

public:
	UPosingGizmoComponent();
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FName SnapshotName = "Anim";
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(ExposeOnSpawn))
	FName BoneName;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	FTransform InitTransform;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	FTransform OffsetTransform;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(ExposeOnSpawn))
	FVector GizmoScale;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(ExposeOnSpawn))
	int DefaultCustomStencil = 2;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(ExposeOnSpawn))
	int SelectCustomStencil = 1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bIsGizmoVisible = true;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	bool bIsDragging = false;

public:
	UFUNCTION(BlueprintCallable)
	void ClearOffset();
	
	void UpdateCustomStencil(const bool& bSelected);
	void UpdateVisible();
	FTransform GetOffsetTransform() const;
	void UpdateInitTransform();

protected:
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
