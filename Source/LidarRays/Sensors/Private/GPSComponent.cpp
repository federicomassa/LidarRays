#include "GPSComponent.h"
#include "OdometryMessage.h"
#include <Engine/World.h>
#include "EngineGlobals.h"
#include <GameFramework/Actor.h>
#include "TazioVehicle.h"
#include "SensorManager.h"
#include <Components/SkeletalMeshComponent.h>
#include <Components/InputComponent.h>
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


UGPSComponent::UGPSComponent() : RandomGenerator(RandomDevice()), PosXRandomNoise(PosXBias, PositionStdDev), PosYRandomNoise(PosYBias, PositionStdDev), YawRandomNoise(0.0, YawStdDev)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	csv_file.open("D:/gps_truth.csv");
}

UGPSComponent::~UGPSComponent()
{
	csv_file.close();
}

void UGPSComponent::BeginPlay()
{
	Super::BeginPlay();

	Owner = Cast<ATazioVehicle>(GetOwner());
	World = GetWorld();
	Mesh = Owner->FindComponentByClass<USkeletalMeshComponent>();

	check(Owner);
	check(World);
	check(Mesh);

	InitLocation = Owner->GetActorLocation();
	InitRotation = Owner->GetActorRotation();
}

void UGPSComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	if (!isActive)
		return;

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

	FRotator CurrentWorldRotationIMU = Owner->GetActorRotation();
	CurrentWorldRotationIMU.Yaw = CurrentWorldRotationIMU.Yaw - 90.f; // mesh is rotated 90 degrees
	FVector WorldAngularVelocity;
	if (Mesh)
		WorldAngularVelocity = Mesh->GetPhysicsAngularVelocityInRadians();

	FVector AngularVelocity = CurrentWorldRotationIMU.UnrotateVector(WorldAngularVelocity);

	// ========================================= BUILD GPS MESSAGE ====================================== //

	//UOdometryMessage* GPSMessage = NewObject<UOdometryMessage>();
	FOdometryMessage GPSMessage;

	// in m, NWU frame
	FVector GPSLocation = InitRotation.UnrotateVector(CurrentWorldLocation - InitLocation)*0.01;
	GPSLocation = FVector(GPSLocation.X, -GPSLocation.Y, GPSLocation.Z);

	FRotator GPSRotation = FRotator(-CurrentWorldRotation.Pitch, -(CurrentWorldRotation.Yaw - InitRotation.Yaw) + 90, CurrentWorldRotation.Roll);
	/*UE_LOG(LogTemp, Warning, TEXT("GPS POSITION: %s"), *GPSLocation.ToString());
	UE_LOG(LogTemp, Warning, TEXT("GPS Orientation: %s"), *GPSRotation.ToString());*/


	// Add noise to measure
	float XCurrentNoise = PosXRandomNoise(RandomGenerator);
	float YCurrentNoise = PosYRandomNoise(RandomGenerator);
	float ZCurrentNoise = 0.f;

	float YawCurrentNoise = YawRandomNoise(RandomGenerator)*PI/180;

	//UE_LOG(LogTemp, Warning, TEXT("Noise: %f, %f"), PositionCurrentNoise, YawCurrentNoise);

	GPSMessage.x = GPSLocation.X + XCurrentNoise;
	GPSMessage.y = GPSLocation.Y + YCurrentNoise;
	GPSMessage.z = GPSLocation.Z + ZCurrentNoise;


	GPSMessage.yaw = (GPSRotation.Yaw)*PI / 180 + YawCurrentNoise;
	GPSMessage.pitch = 0.0f;
	GPSMessage.roll = 0.0f;

	if (GPSMessage.yaw > PI)
		GPSMessage.yaw -= 2 * PI;
	else if (GPSMessage.yaw < -PI)
		GPSMessage.yaw += 2 * PI;

	GPSMessage.yaw_rate = -AngularVelocity.Z;
	//UE_LOG(LogTemp, Warning, TEXT("GPS Yaw: %f"), GPSMessage.yaw);

	FVector Velocity = Owner->GetVelocity();
	//UE_LOG(LogTemp, Warning, TEXT("GPS OWNER VELOCITY: %s"), *Velocity.ToString());

	//UE_LOG(LogTemp, Warning, TEXT("Current World Rotation: %s"), *CurrentWorldRotation.ToString());


	FVector RelativeVelocity = CurrentWorldRotation.UnrotateVector(Velocity);
	RelativeVelocity = FVector(-RelativeVelocity.Y, -RelativeVelocity.X, RelativeVelocity.Z);

	for (int i = 0; i < 36; i++)
	{
		GPSMessage.pose_covariance[i] = 0;

		// Angular velocity in IMU signal
		if (i > 17 && i % 7 == 0)
			GPSMessage.twist_covariance[i] = -1;
		else
			GPSMessage.twist_covariance[i] = 0;
	}

	GPSMessage.vx = RelativeVelocity.X*0.01;
	GPSMessage.vy = RelativeVelocity.Y*0.01;
	GPSMessage.vz = RelativeVelocity.Z*0.01;

	//UE_LOG(LogTemp, Warning, TEXT("Sending velocity linear: %s"), *RelativeVelocity.ToString());

	// Has -1 covariance: not used
	GPSMessage.yaw_rate = 0.f;
	GPSMessage.pitch_rate = 0.f;
	GPSMessage.roll_rate = 0.f;

	
 // ========================================= BUILD GPS MESSAGE ====================================== //

	// Broadcast GPS Message
	Owner->GetSensorManager()->SendGPSMessage(GPSMessage);

// ========================================= BUILD GPS TRUTH MESSAGE ================================= //
	//UOdometryMessage* GPSTruthMessage = NewObject<UOdometryMessage>();
	FOdometryMessage GPSTruthMessage;

	GPSTruthMessage.x = CurrentWorldLocation.X*0.01;
	GPSTruthMessage.y = -CurrentWorldLocation.Y*0.01;
	GPSTruthMessage.z = CurrentWorldLocation.Z*0.01;

	csv_file << GPSTruthMessage.x << "," << GPSTruthMessage.y << '\n';

	// Velocity in truth is relative: x longitudinal, y to the left
	GPSTruthMessage.vx = RelativeVelocity.X*0.01;
	GPSTruthMessage.vy = RelativeVelocity.Y*0.01;
	GPSTruthMessage.vz = RelativeVelocity.Z*0.01;


	GPSTruthMessage.yaw = -Owner->GetActorRotation().Yaw * PI/180;
	GPSTruthMessage.pitch = Owner->GetActorRotation().Pitch * PI/180;
	GPSTruthMessage.roll = Owner->GetActorRotation().Roll * PI/180;

	GPSTruthMessage.yaw_rate = 0.f;
	GPSTruthMessage.pitch_rate = 0.f;
	GPSTruthMessage.roll_rate = 0.f;

	GPSTruthMessage.pose_covariance[0] = FMath::Pow(PositionStdDev,2);
	GPSTruthMessage.pose_covariance[7] = FMath::Pow(PositionStdDev, 2);
	GPSTruthMessage.pose_covariance[14] = 0.f;
	GPSTruthMessage.pose_covariance[21] = 0.f;
	GPSTruthMessage.pose_covariance[28] = 0.f;
	GPSTruthMessage.pose_covariance[35] = FMath::Pow(YawStdDev*PI/180, 2);

	/*UE_LOG(LogTemp, Warning, TEXT("GPS X, Y, Z:  %f, %f, %f"), GPSTruthMessage.x, GPSTruthMessage.y, GPSTruthMessage.z);
	UE_LOG(LogTemp, Warning, TEXT("GPS VX, VY, VZ:  %f, %f, %f"), GPSTruthMessage.vx, GPSTruthMessage.vy, GPSTruthMessage.vz);
	UE_LOG(LogTemp, Warning, TEXT("GPS R, P, Y:  %f, %f, %f"), GPSTruthMessage.roll, -GPSTruthMessage.pitch, -GPSTruthMessage.yaw);*/


	Owner->GetSensorManager()->SendGPSTruthMessage(GPSTruthMessage);
}

void UGPSComponent::ToggleGPS()
{
	isActive = !isActive;
}
