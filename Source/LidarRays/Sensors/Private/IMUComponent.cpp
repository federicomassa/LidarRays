#include "IMUComponent.h"
#include "IMUMessage.h"
#include <Engine/World.h>
#include "EngineGlobals.h"
#include <GameFramework/Actor.h>
#include <Components/SkeletalMeshComponent.h>
#include <DrawDebugHelpers.h>


UIMUComponent::UIMUComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

UIMUComponent::~UIMUComponent()
{
}

void UIMUComponent::BeginPlay()
{
	Super::BeginPlay();

	InitTime = GetWorld()->GetTimeSeconds();

	Owner = GetOwner();
	World = GetWorld();
	Mesh = Owner->FindComponentByClass<USkeletalMeshComponent>();


	if (!Owner)
	{
		UE_LOG(LogTemp, Error, TEXT("IMU component has no owner"));
	}

	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("IMU component has no world"));
	}

	if (!Mesh)
	{
		UE_LOG(LogTemp, Error, TEXT("IMU component has no skeletal mesh"));
	}
}

void UIMUComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//if (DeltaTime < 0.02) return;

	FRotator CurrentWorldRotation = Owner->GetActorRotation();
	CurrentWorldRotation.Yaw = CurrentWorldRotation.Yaw - 90.f; // mesh is rotated 90 degrees

	//FVector CurrentWorldLocation = Owner->GetActorLocation() + CurrentWorldRotation.RotateVector(FVector(0.f, 0.f, 50.f));
	FVector CurrentWorldLocation = Owner->GetActorLocation() + FVector(0.f, 0.f, 50.f); // drawing offset

	// ================   ORIENTATION    ===================== //
	//GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Cyan, FString::Printf(TEXT("r: %f, p: %f, y: %f"), CurrentWorldRotation.Roll, -CurrentWorldRotation.Pitch, -CurrentWorldRotation.Yaw));


	// ======================================================= //


	// ================ ANGULAR VELOCITY ===================== //

	FVector WorldAngularVelocity;
	if (Mesh)
		WorldAngularVelocity = Mesh->GetPhysicsAngularVelocityInRadians();

	FVector AngularVelocity = CurrentWorldRotation.UnrotateVector(WorldAngularVelocity);

	// ======================================================= //


	//GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Cyan, FString::Printf(TEXT("%s"), *AngularVelocity.ToString()));

	//DrawDebugLine(GetWorld(), CurrentWorldLocation, CurrentWorldLocation + WorldAngularVelocity * 10.f, FColor::Red, false, -1.f, (uint8)'\000', 5.f);

	//FVector AngularVelocity = CurrentWorldRotation.UnrotateVector(WorldAngularVelocity);


	/*UE_LOG(LogTemp, Warning, TEXT("Location: %s"), *Owner->GetActorLocation().ToString());
	UE_LOG(LogTemp, Warning, TEXT("Rotation: %s"), *Owner->GetActorRotation().ToString());*/


	// ========================== LINEAR ACCELERATION ============================ //

	float UVx = Owner->GetVelocity().X;
	float UVy = Owner->GetVelocity().Y;
	float UVz = Owner->GetVelocity().Y;

	// Kinematics
	float UAccX = (UVx - LastVx) / DeltaTime;
	float UAccY = (UVy - LastVy) / DeltaTime;
	float UAccZ = (UVz - LastVz) / DeltaTime;

	//UE_LOG(LogTemp, Warning, TEXT("U acc X: %f, Y: %f"), UAccX, UAccY);
	//DrawDebugLine(GetWorld(), CurrentWorldLocation, CurrentWorldLocation + FVector(UAccX, UAccY, 0.f), FColor::Blue, false, -1.f, (uint8)'\000', 10.f);


	LastVx = UVx;
	LastVy = UVy;
	LastVz = UVz;

	FVector WorldAcceleration(UAccX, UAccY, UAccZ);

	// To car frame
	FVector LinearAcceleration = CurrentWorldRotation.UnrotateVector(WorldAcceleration);

	// ============================================================================= //

	// ========================================= BUILD IMU MESSAGE ====================================== //
	//UIMUMessage* IMUReading = NewObject<UIMUMessage>();
	FIMUMessage IMUReading;

	// in rad, NWU frame
	IMUReading.Orientation.x = CurrentWorldRotation.Roll*PI/180;
	IMUReading.Orientation.y = -CurrentWorldRotation.Pitch*PI/180;
	IMUReading.Orientation.z = -CurrentWorldRotation.Yaw*PI/180;

	// in m/s^2, NWU frame
	IMUReading.LinearAcceleration.x = LinearAcceleration.Y*0.01;
	IMUReading.LinearAcceleration.y = LinearAcceleration.X*0.01;
	IMUReading.LinearAcceleration.z = LinearAcceleration.Z*0.01 + 9.81;

	// in rad/s, NWU frame
	IMUReading.AngularVelocity.x = -AngularVelocity.Y; // FIXME I don't know why I need minuses
	IMUReading.AngularVelocity.y = -AngularVelocity.X;
	IMUReading.AngularVelocity.z = -AngularVelocity.Z;

	/*UE_LOG(LogTemp, Warning, TEXT("Lin acc: x = %f, y = %f, z = %f"), IMUReading.LinearAcceleration[0], IMUReading.LinearAcceleration[1], IMUReading.LinearAcceleration[2]);
	UE_LOG(LogTemp, Warning, TEXT("Orient:  r = %f, p = %f, y = %f"), IMUReading.Orientation[0], IMUReading.Orientation[1], IMUReading.Orientation[2]);
	UE_LOG(LogTemp, Warning, TEXT("Ang vel: x = %f, y = %f, z = %f"), IMUReading.AngularVelocity[0], IMUReading.AngularVelocity[1], IMUReading.AngularVelocity[2]);*/

	// Prepare covariances (unknown ==> 0 on the diagonal)
	for (int i = 0; i < 9; i++)
	{
		IMUReading.OrientationCov[i] = 0.f;
		IMUReading.LinearAccelerationCov[i] = 0.f;
		IMUReading.AngularVelocityCov[i] = 0.f;
	}

	// ========================================= BUILD IMU MESSAGE ====================================== //


	// Broadcast IMU Message
	OnIMUAvailable.Broadcast(IMUReading);

	//UE_LOG(LogTemp, Warning, TEXT("Inserting: x: %f, y: %f, z: %f"), LinearAcceleration.X, LinearAcceleration.Y, LinearAcceleration.Z);
	//UE_LOG(LogTemp, Warning, TEXT("Inserting: x: %f, y: %f, z: %f"), IMUReading.AngularVelocity[0], IMUReading.AngularVelocity[1], IMUReading.AngularVelocity[2]);



}