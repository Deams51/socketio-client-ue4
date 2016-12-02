#pragma once

#include "sio_client.h"
#include "SocketIOClientTypes.h"

class SOCKETIOCLIENT_API FSIOClient
{

public:
	FSIOClient();
	~FSIOClient();

	
	//Async events

	/** Event received on socket.io connection established. */
	FSIOCOpenEventSignatureBase OnConnected;

	/** Event received on socket.io connection disconnected. */
	FSIOCCloseEventSignatureBase OnDisconnected;

	/** Event received on having joined namespace. */
	FSIOCSocketEventSignatureBase OnSocketNamespaceConnected;

	/** Event received on having left namespace. */
	FSIOCSocketEventSignatureBase OnSocketNamespaceDisconnected;

	/** Event received on connection failure. */
	FSIOCEventSignatureBase OnFail;

	/** On bound event received. */
	FSIOCEventJsonSignatureBase On;

	/** Default connection address string in form e.g. http://localhost:80. */
	FString AddressAndPort;

	/** When connected this session id will be valid and contain a unique Id. */
	FString SessionId;

	/**
	* Connect to a socket.io server, optional if auto-connect is set
	*
	* @param AddressAndPort	the address in URL format with port
	*/
	void Connect(const FString& InAddressAndPort);
	void Disconnect();
	void SyncDisconnect();

	void Emit(const FString& EventName, USIOJsonValue* Message = nullptr, const FString& Namespace = FString(TEXT("/")));

	/**
	* Bind an event, then respond to it with 'On' multi-cast delegate
	*
	* @param EventName	Event name
	* @param Namespace	Optional namespace, defaults to default namespace
	*/
	void BindEvent(const FString& EventName, const FString& Namespace = FString(TEXT("/")));

	/**
	* Emit an event with a JsonValue message
	*
	* @param EventName				Event name
	* @param Message				FJsonValue
	* @param CallbackFunction		Optional callback TFunction
	* @param Namespace				Optional Namespace within socket.io
	*/
	void EmitNative(const FString& EventName,
					const TSharedPtr<FJsonValue>& Message = nullptr,
					TFunction< void(const TArray<TSharedPtr<FJsonValue>>&)> CallbackFunction = nullptr,
					const FString& Namespace = FString(TEXT("/")));

	/**
	* (Overloaded) Emit an event with a Json Object message
	*
	* @param EventName				Event name
	* @param ObjectMessage			FJsonObject
	* @param CallbackFunction		Optional callback TFunction
	* @param Namespace				Optional Namespace within socket.io
	*/
	void EmitNative(const FString& EventName,
					const TSharedPtr<FJsonObject>& ObjectMessage = nullptr,
					TFunction< void(const TArray<TSharedPtr<FJsonValue>>&)> CallbackFunction = nullptr,
					const FString& Namespace = FString(TEXT("/")));

	/**
	* (Overloaded) Emit an event with a string message
	*
	* @param EventName				Event name
	* @param StringMessage			Message in string format
	* @param CallbackFunction		Optional callback TFunction
	* @param Namespace				Optional Namespace within socket.io
	*/
	void EmitNative(const FString& EventName,
					const FString& StringMessage = FString(),
					TFunction< void(const TArray<TSharedPtr<FJsonValue>>&)> CallbackFunction = nullptr,
					const FString& Namespace = FString(TEXT("/")));

	/**
	* (Overloaded) Emit an event with a number (double) message
	*
	* @param EventName				Event name
	* @param NumberMessage			Message in double format
	* @param CallbackFunction		Optional callback TFunction
	* @param Namespace				Optional Namespace within socket.io
	*/
	void EmitNative(const FString& EventName,
					double NumberMessage,
					TFunction< void(const TArray<TSharedPtr<FJsonValue>>&)> CallbackFunction = nullptr,
					const FString& Namespace = FString(TEXT("/")));

	/**
	* (Overloaded) Emit an event with a bool message
	*
	* @param EventName				Event name
	* @param BooleanMessage			Message in bool format
	* @param CallbackFunction		Optional callback TFunction
	* @param Namespace				Optional Namespace within socket.io
	*/
	void EmitNative(const FString& EventName,
					bool BooleanMessage,
					TFunction< void(const TArray<TSharedPtr<FJsonValue>>&)> CallbackFunction = nullptr,
					const FString& Namespace = FString(TEXT("/")));

	/**
	* (Overloaded) Emit an event with a binary message
	*
	* @param EventName				Event name
	* @param BinaryMessage			Message in an TArray of uint8
	* @param CallbackFunction		Optional callback TFunction
	* @param Namespace				Optional Namespace within socket.io
	*/
	void EmitNative(const FString& EventName,
					const TArray<uint8>& BinaryMessage,
					TFunction< void(const TArray<TSharedPtr<FJsonValue>>&)> CallbackFunction = nullptr,
					const FString& Namespace = FString(TEXT("/")));

	/**
	* (Overloaded) Emit an event with an array message
	*
	* @param EventName				Event name
	* @param ArrayMessage			Message in an TArray of FJsonValues
	* @param CallbackFunction		Optional callback TFunction
	* @param Namespace				Optional Namespace within socket.io
	*/
	void EmitNative(const FString& EventName,
					const TArray<TSharedPtr<FJsonValue>>& ArrayMessage,
					TFunction< void(const TArray<TSharedPtr<FJsonValue>>&)> CallbackFunction = nullptr,
					const FString& Namespace = FString(TEXT("/")));

	/**
	* (Overloaded) Emit an event with an UStruct message
	*
	* @param EventName				Event name
	* @param Struct					UStruct type usually obtained via e.g. FMyStructType::StaticStruct()
	* @param StructPtr				Pointer to the actual struct memory e.g. &MyStruct
	* @param CallbackFunction		Optional callback TFunction
	* @param Namespace				Optional Namespace within socket.io
	*/
	void EmitNative(const FString& EventName,
					UStruct* Struct,
					const void* StructPtr,
					TFunction< void(const TArray<TSharedPtr<FJsonValue>>&)> CallbackFunction = nullptr,
					const FString& Namespace = FString(TEXT("/")));

	/**
	* Emit a raw sio::message event
	*
	* @param EventName				Event name
	* @param MessageList			Message in sio::message::list format
	* @param CallbackFunction		Optional callback TFunction with raw signature
	* @param Namespace				Optional Namespace within socket.io
	*/
	void EmitRaw(const FString& EventName,
				 const sio::message::list& MessageList = nullptr,
				 TFunction<void(const sio::message::list&)> CallbackFunction = nullptr,
				 const FString& Namespace = FString(TEXT("/")));

	/**
	* Emit an optimized binary message
	*
	* @param EventName				Event name
	* @param Data					Buffer Pointer
	* @param DataLength				Buffer size
	* @param Namespace				Optional Namespace within socket.io
	*/
	void EmitRawBinary(const FString& EventName, uint8* Data, int32 DataLength, const FString& Namespace = FString(TEXT("/")));


	/**
	* Call function callback on receiving socket event. C++ only.
	*
	* @param EventName	Event name
	* @param TFunction	Lambda callback, JSONValue
	* @param Namespace	Optional namespace, defaults to default namespace
	*/
	void OnNativeEvent(const FString& EventName,
					   TFunction< void(const FString&, const TSharedPtr<FJsonValue>&)> CallbackFunction,
					   const FString& Namespace = FString(TEXT("/")));

	/**
	* Call function callback on receiving raw event. C++ only.
	*
	* @param EventName	Event name
	* @param TFunction	Lambda callback, raw flavor
	* @param Namespace	Optional namespace, defaults to default namespace
	*/
	void OnRawEvent(const FString& EventName,
					TFunction< void(const FString&, const sio::message::ptr&)> CallbackFunction,
					const FString& Namespace = FString(TEXT("/")));
	/**
	* Call function callback on receiving binary event. C++ only.
	*
	* @param EventName	Event name
	* @param TFunction	Lambda callback, raw flavor
	* @param Namespace	Optional namespace, defaults to default namespace
	*/
	void OnBinaryEvent(const FString& EventName,
					   TFunction< void(const FString&, const TArray<uint8>&)> CallbackFunction,
					   const FString& Namespace = FString(TEXT("/")));

protected:
	sio::client PrivateClient;

};