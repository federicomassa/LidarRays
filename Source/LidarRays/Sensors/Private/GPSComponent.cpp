#include "GPSComponent.h"
#include "OdometryMessage.h"
#include "OdometrySimulinkMessage.h"
#include <Engine/World.h>
#include "EngineGlobals.h"
#include <GameFramework/Actor.h>
#include <Components/SkeletalMeshComponent.h>
#include <DrawDebugHelpers.h>

#ifndef SIMULINK
#define SIMULINK
#endif

//
//#ifdef WIN32
//#include "AllowWindowsPlatformTypes.h"
//#include "Windows.h"
//#include "HideWindowsPlatformTypes.h"
//#else
//#error "FIXME Windows platform needed for timers!"
//#endif


UGPSComponent::UGPSComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

UGPSComponent::~UGPSComponent()
{
}

void UGPSComponent::BeginPlay()
{
	Super::BeginPlay();

	Owner = GetOwner();
	World = GetWorld();
	Mesh = Owner->FindComponentByClass<USkeletalMeshComponent>();

	InitLocation = Owner->GetActorLocation();
	InitRotation = Owner->GetActorRotation();

	if (!Owner)
	{
		UE_LOG(LogTemp, Error, TEXT("GPS component has no owner"));
	}

	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("GPS component has no world"));
	}

	if (!Mesh)
	{
		UE_LOG(LogTemp, Error, TEXT("GPS component has no skeletal mesh"));
	}
}

void UGPSComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//if (DeltaTime < 0.02) return;

	//_LARGE_INTEGER StartingTime;
	//QueryPerformanceCounter(&StartingTime);
	//_LARGE_INTEGER SystemTickFrequency;
	//QueryPerformanceFrequency(&SystemTickFrequency);

	FRotator CurrentWorldRotation = Owner->GetActorRotation();
	CurrentWorldRotation.Yaw = CurrentWorldRotation.Yaw + 90; // mesh is rotated 90 degrees? (here is +, in IMU is -??)

	//FVector CurrentWorldLocation = Owner->GetActorLocation() + CurrentWorldRotation.RotateVector(FVector(0.f, 0.f, 50.f));
	FVector CurrentWorldLocation = Owner->GetActorLocation();

	// ================   ORIENTATION    ===================== //
	//GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Cyan, FString::Printf(TEXT("r: %f, p: %f, y: %f"), CurrentWorldRotation.Roll, -CurrentWorldRotation.Pitch, -CurrentWorldRotation.Yaw));


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

	// ========================================= BUILD GPS MESSAGE ====================================== //
#ifndef SIMULINK
	UOutgoingMessage* GPSMessage = NewObject<UOutgoingMessage>();
#else
	UOutgoingSimulinkMessage* GPSMessage = NewObject<UOutgoingSimulinkMessage>();
#endif

#ifndef SIMULINK
	OdometryMessage<cereal::BinaryOutputArchive>* Data = new OdometryMessage<cereal::BinaryOutputArchive>;
#else
	OdometrySimulinkMessage<simulink::SimulinkOutputArchive>* Data = new OdometrySimulinkMessage<simulink::SimulinkOutputArchive>;
#endif

#ifndef SIMULINK
	Data->Timestamp = World->GetTimeSeconds();
#endif

	// in m, NWU frame
	FVector GPSLocation = InitRotation.UnrotateVector(CurrentWorldLocation - InitLocation)*0.01;
	GPSLocation = FVector(GPSLocation.X, -GPSLocation.Y, GPSLocation.Z);

	FRotator GPSRotation = FRotator(-CurrentWorldRotation.Pitch, -(CurrentWorldRotation.Yaw - InitRotation.Yaw) + 90, CurrentWorldRotation.Roll);
	/*UE_LOG(LogTemp, Warning, TEXT("GPS POSITION: %s"), *GPSLocation.ToString());
	UE_LOG(LogTemp, Warning, TEXT("GPS Orientation: %s"), *GPSRotation.ToString());*/

#ifndef SIMULINK
	Data->PoseWithCovariance.Pose.X = GPSLocation.X;
	Data->PoseWithCovariance.Pose.Y = GPSLocation.Y;
	Data->PoseWithCovariance.Pose.Z = GPSLocation.Z;

	Data->PoseWithCovariance.Pose.Roll = GPSRotation.Roll*PI / 180;
	Data->PoseWithCovariance.Pose.Pitch = GPSRotation.Pitch*PI / 180;
	Data->PoseWithCovariance.Pose.Yaw = GPSRotation.Yaw*PI / 180;
#else
	Data->X = GPSLocation.X;
	Data->Y = GPSLocation.Y;
	Data->Z = GPSLocation.Z;
	Data->Phi = GPSRotation.Yaw*PI / 180;
#endif

	FVector Velocity = Owner->GetVelocity();
	FVector RelativeVelocity = CurrentWorldRotation.UnrotateVector(Velocity);
	RelativeVelocity = FVector(-RelativeVelocity.Y, -RelativeVelocity.X, RelativeVelocity.Z);

#ifndef SIMULINK
	for (int i = 0; i < 36; i++)
	{
		Data->PoseWithCovariance.Covariance[i] = 0;

		// Angular velocity in IMU signal
		if (i > 17 && i % 7 == 0)
			Data->TwistWithCovariance.Covariance[i] = -1;
		else
			Data->TwistWithCovariance.Covariance[i] = 0;
	}



	Data->TwistWithCovariance.Twist.Linear[0] = RelativeVelocity.X*0.01;
	Data->TwistWithCovariance.Twist.Linear[1] = RelativeVelocity.Y*0.01;
	Data->TwistWithCovariance.Twist.Linear[2] = RelativeVelocity.Z*0.01;

	//UE_LOG(LogTemp, Warning, TEXT("Sending velocity linear: %s"), *RelativeVelocity.ToString());

	// Has -1 covariance: not used
	Data->TwistWithCovariance.Twist.Angular[0] = 0.f;
	Data->TwistWithCovariance.Twist.Angular[1] = 0.f;
	Data->TwistWithCovariance.Twist.Angular[2] = 0.f;
#else
	Data->VX = RelativeVelocity.X*0.01;
	Data->VY = RelativeVelocity.Y*0.01;
	Data->VZ = RelativeVelocity.Z*0.01;
#endif

	// ================ ANGULAR VELOCITY ===================== //

	/*FVector WorldAngularVelocity;
	if (Mesh)
		WorldAngularVelocity = Mesh->GetPhysicsAngularVelocityInRadians();

	FVector AngularVelocity = Owner->GetActorRotation().UnrotateVector(WorldAngularVelocity);
*/
//UE_LOG(LogTemp, Warning, TEXT("Angular: %s"), *AngularVelocity.ToString());

// ======================================================= //

/*UE_LOG(LogTemp, Warning, TEXT("Velocity: %s"), *Velocity.ToString());
UE_LOG(LogTemp, Warning, TEXT("Relative Velocity: %s"), *RelativeVelocity.ToString());
*/


//Data->Orientation.push_back(CurrentWorldRotation.Roll*PI/180);
//Data->Orientation.push_back(-CurrentWorldRotation.Pitch*PI/180);
//Data->Orientation.push_back(-CurrentWorldRotation.Yaw*PI/180);

//// in m/s^2, NWU frame
//Data->LinearAcceleration.push_back(LinearAcceleration.Y*0.01);
//Data->LinearAcceleration.push_back(LinearAcceleration.X*0.01);
//Data->LinearAcceleration.push_back(LinearAcceleration.Z*0.01 + 9.81);

//// in rad/s, NWU frame
//Data->AngularVelocity.push_back(AngularVelocity.Y);
//Data->AngularVelocity.push_back(AngularVelocity.X);
//Data->AngularVelocity.push_back(AngularVelocity.Z);

//// Prepare covariances (unknown ==> 0 on the diagonal)
//for (int i = 0; i < 9; i++)
//{
//	if (i == 0 || i == 4 || i == 8)
//	{
//		Data->OrientationCov.push_back(0.f);
//		Data->LinearAccelerationCov.push_back(0.f);
//		Data->AngularVelocityCov.push_back(0.f);
//	}
//	else
//	{
//		Data->OrientationCov.push_back(0.f);
//		Data->LinearAccelerationCov.push_back(0.f);
//		Data->AngularVelocityCov.push_back(0.f);
//	}
//}

// ========================================= BUILD GPS MESSAGE ====================================== //

	GPSMessage->message = Data;
	// Broadcast GPS Message
	OnGPSAvailable.Broadcast(GPSMessage);

	/*_LARGE_INTEGER EndTime;
	QueryPerformanceCounter(&EndTime);

	_LARGE_INTEGER Ticks;
	Ticks.QuadPart = (EndTime.QuadPart - StartingTime.QuadPart);
	Ticks.QuadPart *= 1000000;
	Ticks.QuadPart /= SystemTickFrequency.QuadPart;

	int ElapsedMicroseconds = Ticks.QuadPart;
	UE_LOG(LogTemp, Warning, TEXT("GPS took %i microseconds"), ElapsedMicroseconds);
*/
//UE_LOG(LogTemp, Warning, TEXT("Inserting: x: %f, y: %f, z: %f"), LinearAcceleration.X, LinearAcceleration.Y, LinearAcceleration.Z);
//UE_LOG(LogTemp, Warning, TEXT("Inserting: x: %f, y: %f, z: %f"), Data->AngularVelocity[0], Data->AngularVelocity[1], Data->AngularVelocity[2]);


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
//		UOdometryMessage* Data = NewObject<UOdometryMessage>();
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
//		Data->LinearAcceleration.push_back(acc_x);
//
//		/*UE_LOG(LogTemp, Warning, TEXT("GPS X: %f"), LinearAcceleration.X);
//		UE_LOG(LogTemp, Warning, TEXT("GPS Y: %f"), LinearAcceleration.Y);
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
////		UE_LOG(LogTemp, Warning, TEXT("GPS X: %f, %f, %f, %f"), x3, x2, t3, t2);
//	}
//
}
