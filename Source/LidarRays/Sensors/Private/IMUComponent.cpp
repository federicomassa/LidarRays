#include "IMUComponent.h"
#include "IMUMessage.h"
#include <Engine/World.h>
#include "EngineGlobals.h"
#include <GameFramework/Actor.h>
#include <Components/SkeletalMeshComponent.h>
#include "SensorManager.h"
#include "TazioVehicle.h"
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

	Owner = Cast<ATazioVehicle>(GetOwner());
	World = GetWorld();

	check(Owner);
	check(World);

	Mesh = Owner->FindComponentByClass<USkeletalMeshComponent>();

	check(Mesh);
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


	//GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Cyan, FString::Printf(TEXT("%s"), *AngularVelocity.ToString()));

	//DrawDebugLine(GetWorld(), CurrentWorldLocation, CurrentWorldLocation + WorldAngularVelocity * 10.f, FColor::Red, false, -1.f, (uint8)'\000', 5.f);

	//FVector AngularVelocity = CurrentWorldRotation.UnrotateVector(WorldAngularVelocity);


	/*UE_LOG(LogTemp, Warning, TEXT("Location: %s"), *Owner->GetActorLocation().ToString());
	UE_LOG(LogTemp, Warning, TEXT("Rotation: %s"), *Owner->GetActorRotation().ToString());*/


	// ========================== LINEAR ACCELERATION ============================ //

	float UVx = Owner->GetVelocity().X;
	float UVy = Owner->GetVelocity().Y;
	float UVz = Owner->GetVelocity().Y;
	float UPitch = CurrentWorldRotation.Pitch;
	float UYaw = CurrentWorldRotation.Yaw;
	float URoll = CurrentWorldRotation.Roll;

	// Kinematics
	float UAccX = (UVx - LastVx) / DeltaTime;
	float UAccY = (UVy - LastVy) / DeltaTime;
	float UAccZ = (UVz - LastVz) / DeltaTime;

	float UPitchDiff = UPitch - LastPitch;
	float UYawDiff = UYaw - LastYaw;
	float URollDiff = URoll - LastRoll;

	//UE_LOG(LogTemp, Warning, TEXT("Angle diff deg: %f, %f, %f"), URollDiff, UPitchDiff, UYawDiff);

	if (UPitchDiff > 180.f)
		UPitchDiff -= 360.f;
	else if (UPitchDiff < -180.f)
		UPitchDiff += 360.f;

	if (UYawDiff > 180.f)
		UYawDiff -= 360.f;
	else if (UYawDiff < -180.f)
		UYawDiff += 360.f;

	if (URollDiff > 180.f)
		URollDiff -= 360.f;
	else if (URollDiff < -180.f)
		URollDiff += 360.f;

	float UPitchRate = (UPitchDiff) / DeltaTime;
	float UYawRate = (UYawDiff) / DeltaTime;
	float URollRate = (URollDiff) / DeltaTime;

	//UE_LOG(LogTemp, Warning, TEXT("U acc X: %f, Y: %f"), UAccX, UAccY);
	//DrawDebugLine(GetWorld(), CurrentWorldLocation, CurrentWorldLocation + FVector(UAccX, UAccY, 0.f), FColor::Blue, false, -1.f, (uint8)'\000', 10.f);


	// ================ ANGULAR VELOCITY ===================== //
	FVector WorldAngularVelocity;
	WorldAngularVelocity.X = URollRate;
	WorldAngularVelocity.Y = UPitchRate;
	WorldAngularVelocity.Z = UYawRate;

	FVector AngularVelocity = CurrentWorldRotation.UnrotateVector(WorldAngularVelocity);

	// ======================================================= //
	


	LastVx = UVx;
	LastVy = UVy;
	LastVz = UVz;
	LastPitch = UPitch;
	LastYaw = UYaw;
	LastRoll = URoll;

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
	//UE_LOG(LogTemp, Warning, TEXT("Angular velocity in deg: %f, %f, %f"), -AngularVelocity.Y, -AngularVelocity.X, -AngularVelocity.Z);

	IMUReading.AngularVelocity.x = -AngularVelocity.Y*PI/180; // FIXME I don't know why I need minuses
	IMUReading.AngularVelocity.y = -AngularVelocity.X*PI/180;
	IMUReading.AngularVelocity.z = -AngularVelocity.Z*PI/180;

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
	Owner->GetSensorManager()->SendIMUMessage(IMUReading);

	//UE_LOG(LogTemp, Warning, TEXT("Inserting: x: %f, y: %f, z: %f"), LinearAcceleration.X, LinearAcceleration.Y, LinearAcceleration.Z);
	//UE_LOG(LogTemp, Warning, TEXT("Inserting: x: %f, y: %f, z: %f"), IMUReading.AngularVelocity[0], IMUReading.AngularVelocity[1], IMUReading.AngularVelocity[2]);



}