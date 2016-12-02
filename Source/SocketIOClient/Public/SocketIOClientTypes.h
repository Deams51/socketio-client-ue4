#pragma once

#include "SocketIOClientTypes.generated.h"


UENUM(BlueprintType)
enum EMessageTypeFlag
{
	FLAG_INTEGER,
	FLAG_DOUBLE,
	FLAG_STRING,
	FLAG_BINARY,
	FLAG_ARRAY,
	FLAG_OBJECT,
	FLAG_BOOLEAN,
	FLAG_NULL
};

//TODO: convert sio::message to UE struct for more flexible use
USTRUCT()
struct FSIOMessage
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY(BlueprintReadWrite, Category = "SocketIO Message Properties")
		TEnumAsByte<EMessageTypeFlag> MessageFlag;

	//Internal UE storage
	FJsonObject Object;
};

UENUM(BlueprintType)
enum EConnectionCloseReason
{
	CLOSE_REASON_NORMAL,
	CLOSE_REASON_DROP
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSIOCEventSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSIOCSocketEventSignature, FString, Namespace);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSIOCOpenEventSignature, FString, SessionId);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSIOCCloseEventSignature, TEnumAsByte<EConnectionCloseReason>, Reason);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSIOCEventJsonSignature, FString, Event, class USIOJsonValue*, MessageJson);

DECLARE_DELEGATE(FSIOCEventSignatureBase);
DECLARE_DELEGATE_OneParam(FSIOCSocketEventSignatureBase, FString);
DECLARE_DELEGATE_OneParam(FSIOCOpenEventSignatureBase, FString);
DECLARE_DELEGATE_OneParam(FSIOCCloseEventSignatureBase, TEnumAsByte<EConnectionCloseReason>);
DECLARE_DELEGATE_TwoParams(FSIOCEventJsonSignatureBase, FString, USIOJsonValue*);
