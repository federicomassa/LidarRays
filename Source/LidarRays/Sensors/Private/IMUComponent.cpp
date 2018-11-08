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


	//// Yaw solidal axes in ENU
	//FVector AxisX = FRotator(0.f, CurrentWorldRotation.Yaw - 90.f, 0.f).RotateVector(FVector(0.f, 1.f, 0.f));
	//FVector AxisY = FRotator(0.f, CurrentWorldRotation.Yaw - 90.f, 0.f).RotateVector(FVector(1.f, 0.f, 0.f));
	//FVector AxisZ = FVector(0.f, 0.f, 1.f);

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
	UIMUMessage* IMUReading = NewObject<UIMUMessage>();

	// in rad, NWU frame
	IMUReading->Orientation.push_back(CurrentWorldRotation.Roll*PI/180);
	IMUReading->Orientation.push_back(-CurrentWorldRotation.Pitch*PI/180);
	IMUReading->Orientation.push_back(-CurrentWorldRotation.Yaw*PI/180);

	// in m/s^2, NWU frame
	IMUReading->LinearAcceleration.push_back(LinearAcceleration.Y*0.01);
	IMUReading->LinearAcceleration.push_back(LinearAcceleration.X*0.01);
	IMUReading->LinearAcceleration.push_back(LinearAcceleration.Z*0.01 + 9.81);

	// in rad/s, NWU frame
	IMUReading->AngularVelocity.push_back(AngularVelocity.Y);
	IMUReading->AngularVelocity.push_back(AngularVelocity.X);
	IMUReading->AngularVelocity.push_back(AngularVelocity.Z);

	// Prepare covariances (unknown ==> 0 on the diagonal)
	for (int i = 0; i < 9; i++)
	{
		if (i == 0 || i == 4 || i == 8)
		{
			IMUReading->OrientationCov.push_back(0.f);
			IMUReading->LinearAccelerationCov.push_back(0.f);
			IMUReading->AngularVelocityCov.push_back(0.f);
		}
		else
		{
			IMUReading->OrientationCov.push_back(0.f);
			IMUReading->LinearAccelerationCov.push_back(0.f);
			IMUReading->AngularVelocityCov.push_back(0.f);
		}
	}

	// ========================================= BUILD IMU MESSAGE ====================================== //


	// Broadcast IMU Message
	OnIMUAvailable.Broadcast(IMUReading);

	//UE_LOG(LogTemp, Warning, TEXT("Inserting: x: %f, y: %f, z: %f"), LinearAcceleration.X, LinearAcceleration.Y, LinearAcceleration.Z);
	//UE_LOG(LogTemp, Warning, TEXT("Inserting: x: %f, y: %f, z: %f"), IMUReading->AngularVelocity[0], IMUReading->AngularVelocity[1], IMUReading->AngularVelocity[2]);


	// Draw axes
	/*DrawDebugLine(GetWorld(), CurrentWorldLocation, CurrentWorldLocation + AxisX * 100.f, FColor::Red, false, -1.f, (uint8)'\000', 2.f);
	DrawDebugLine(GetWorld(), CurrentWorldLocation, CurrentWorldLocation + AxisY * 100.f, FColor::Green, false, -1.f, (uint8)'\000', 2.f);
	DrawDebugLine(GetWorld(), CurrentWorldLocation, CurrentWorldLocation + AxisZ * 100.f, FColor::Yellow, false, -1.f, (uint8)'\000', 2.f);
*/
	//DrawDebugLine(GetWorld(), CurrentWorldLocation, CurrentWorldLocation + FRotator(0.f, CurrentWorldRotation.Yaw - 90.f, 0.f).RotateVector(FVector(0.f, 1.f, 0.f)) * 100.f, FColor::Yellow, false, -1.f, (uint8)'\000', 2.f);

	// Save current pose in Unreal frame. This is a world pose 
//	UPoseStampedMessage* CurrentPose = NewObject<UPoseStampedMessage>();
//	CurrentPose->Timestamp = GetWorld()->GetTimeSeconds();
//	
//	CurrentPose->X = Owner->GetActorLocation().X;
//	CurrentPose->Y = Owner->GetActorLocation().Y;
//	CurrentPose->Z = Owner->GetActorLocation().Z;
//
//	CurrentPose->Roll = Owner->GetActorRotation().Roll;
//	CurrentPose->Pitch = Owner->GetActorRotation().Pitch;
//	CurrentPose->Yaw = Owner->GetActorRotation().Yaw - 90.f; // FIXME 90 because mesh is oriented towards X
//
//	FRotator CurrentRotator = FRotator(CurrentPose->Pitch, CurrentPose->Yaw, CurrentPose->Roll);
//
//	// Yaw in ]-180, 180] from ]-90, 270]
//	if (CurrentPose->Yaw <= 270 && CurrentPose->Yaw >= 180)
//		CurrentPose->Yaw -= 360;
//
//	// Insert current pose at the top of the array
//	PastPoses.Insert(CurrentPose, 0);
//	
//	if (PastPoses.Num() > 4)
//		PastPoses.RemoveAt(4);
//
//	if (PastPoses.Num() == 4)
//	{
//		// Ready to compute angular velocity and linear acceleration
//		UIMUMessage* IMUReading = NewObject<UIMUMessage>();
//
//		// Linear acceleration in solidal frame (X = ahead, Y = on the left)
//		FVector RotatedPose1 = FVector(PastPoses[3]->X, PastPoses[3]->Y, PastPoses[3]->Z);
//		float x1 = RotatedPose1.X;
//		float y1 = RotatedPose1.Y;
//		float t1 = PastPoses[3]->Timestamp;
//
//		FVector RotatedPose2 = FVector(PastPoses[2]->X, PastPoses[2]->Y, PastPoses[2]->Z);
//		float x2 = RotatedPose2.X;
//		float y2 = RotatedPose2.Y;
//		float t2 = PastPoses[2]->Timestamp;
//
//		FVector RotatedPose3 = FVector(PastPoses[1]->X, PastPoses[1]->Y, PastPoses[1]->Z);
//		float x3 = RotatedPose3.X;
//		float y3 = RotatedPose3.Y;
//		float t3 = PastPoses[1]->Timestamp;
//
//		FVector RotatedPose4 = FVector(PastPoses[0]->X, PastPoses[0]->Y, PastPoses[0]->Z);
//		float x4 = RotatedPose4.X;
//		float y4 = RotatedPose4.Y;
//		float t4 = PastPoses[0]->Timestamp;
//
//		// Euler central
//		float vx2 = (x3 - x1) / (t3 - t1);
//		float vy2 = (y3 - y1) / (t3 - t1);
//
//		float vx3 = (x4 - x2) / (t4 - t2);
//		float vy3 = (y4 - y2) / (t4 - t2);
//
//		/*float acc_x = ((x3 - x2) / (t3 - t2) - (x2 - x1) / (t2 - t1)) / (t3 - t2);
//		float acc_y = ((y3 - y2) / (t3 - t2) - (y2 - y1) / (t2 - t1)) / (t3 - t2);
//*/
//		float acc_x = (vx3 - vx2) / (t3 - t2);
//		float acc_y = (vy3 - vy2) / (t3 - t2);
//		float acc_z = 0.f; // FIXME
//
//		FVector WorldAcceleration(acc_x, acc_y, acc_z);
//
//		//DrawDebugLine(GetWorld(), CurrentWorldLocation, CurrentWorldLocation + WorldAcceleration, FColor::Blue, false, -1.f, (uint8)'\000', 10.f);
//
//		FVector LinearAcceleration = CurrentRotator.RotateVector(WorldAcceleration);
//
//
//		IMUReading->LinearAcceleration.push_back(acc_x);
//
//		/*UE_LOG(LogTemp, Warning, TEXT("IMU X: %f"), LinearAcceleration.X);
//		UE_LOG(LogTemp, Warning, TEXT("IMU Y: %f"), LinearAcceleration.Y);
//
//		UE_LOG(LogTemp, Warning, TEXT("My velocity X: %f, Y: %f"), vx2, vy2);*/
//
//		
//
//		/*UE_LOG(LogTemp, Warning, TEXT("t1: %f, t2: %f, t3: %f"), t1, t2, t3);
//		UE_LOG(LogTemp, Warning, TEXT("x1: %f, x2: %f, x3: %f"), x1, x2, x3);
//		UE_LOG(LogTemp, Warning, TEXT("y1: %f, y2: %f, y3: %f"), y1, y2, y3);*/
//
//
////		UE_LOG(LogTemp, Warning, TEXT("IMU X: %f, %f, %f, %f"), x3, x2, t3, t2);
//	}
//
}