// Fill out your copyright notice in the Description page of Project Settings.

#include "SocketIOClientPrivatePCH.h"
#include "SIOLambdaRunnable.h"
#include "SIOJConvert.h"
#include "SIOClient.h"
#include <string>

FSIOClient::FSIOClient()
{
	AddressAndPort = FString(TEXT("http://localhost:3000"));	//default to 127.0.0.1
	SessionId = FString(TEXT("invalid"));
}

FSIOClient::~FSIOClient()
{
	// Disconnect if needed before destroying
	SyncDisconnect();
}

#pragma region Connect

void FSIOClient::Connect(const FString& InAddressAndPort)
{
	std::string StdAddressString = USIOMessageConvert::StdString(InAddressAndPort);
	if (InAddressAndPort.IsEmpty())
	{
		StdAddressString = USIOMessageConvert::StdString(AddressAndPort);
	}

	//Connect to the server on a background thread
	FSIOLambdaRunnable::RunLambdaOnBackGroundThread([&]
	{
		//Attach the specific connection status events events

		PrivateClient.set_open_listener(sio::client::con_listener([&]() {
			SessionId = USIOMessageConvert::FStringFromStd(PrivateClient.get_sessionid());
			UE_LOG(SocketIOLog, Log, TEXT("SocketIO Connected with session: %s"), *SessionId);
			OnConnected.ExecuteIfBound(SessionId);
		}));

		PrivateClient.set_close_listener(sio::client::close_listener([&](sio::client::close_reason const& reason)
		{
			SessionId = FString(TEXT("invalid"));
			UE_LOG(SocketIOLog, Log, TEXT("SocketIO Disconnected"));
			OnDisconnected.ExecuteIfBound((EConnectionCloseReason)reason);
		}));

		PrivateClient.set_socket_open_listener(sio::client::socket_listener([&](std::string const& nsp)
		{
			FString Namespace = USIOMessageConvert::FStringFromStd(nsp);
			UE_LOG(SocketIOLog, Log, TEXT("SocketIO connected to namespace: %s"), *Namespace);
			OnSocketNamespaceConnected.ExecuteIfBound(Namespace);
		}));

		PrivateClient.set_socket_close_listener(sio::client::socket_listener([&](std::string const& nsp)
		{
			FString Namespace = USIOMessageConvert::FStringFromStd(nsp);
			UE_LOG(SocketIOLog, Log, TEXT("SocketIO disconnected from namespace: %s"), *Namespace);
			OnSocketNamespaceDisconnected.ExecuteIfBound(USIOMessageConvert::FStringFromStd(nsp));
		}));

		PrivateClient.set_fail_listener(sio::client::con_listener([&]()
		{
			UE_LOG(SocketIOLog, Log, TEXT("SocketIO failed to connect."));
			OnFail.ExecuteIfBound();
		}));

		PrivateClient.connect(StdAddressString);
	});
}


void FSIOClient::Disconnect()
{
	FSIOLambdaRunnable::RunLambdaOnBackGroundThread([&]
	{
		SyncDisconnect();
	});
}

void FSIOClient::SyncDisconnect()
{
	if (PrivateClient.opened())
	{
		PrivateClient.socket()->off_all();
		PrivateClient.socket()->off_error();
		PrivateClient.close();
	}
}

#pragma endregion Connect


#pragma region Emit

void FSIOClient::Emit(const FString& EventName, USIOJsonValue* Message, const FString& Namespace /*= FString(TEXT("/"))*/)
{
	PrivateClient.socket(USIOMessageConvert::StdString(Namespace))->emit(
		USIOMessageConvert::StdString(EventName),
		USIOMessageConvert::ToSIOMessage(Message->GetRootValue()));
}

void FSIOClient::EmitNative(const FString& EventName, const TSharedPtr<FJsonValue>& Message /*= nullptr*/, TFunction< void(const TArray<TSharedPtr<FJsonValue>>&)> CallbackFunction /*= nullptr*/, const FString& Namespace /*= FString(TEXT("/"))*/)
{
	const auto SafeCallback = CallbackFunction;
	EmitRaw(
		EventName,
		USIOMessageConvert::ToSIOMessage(Message),
		[&, SafeCallback](const sio::message::list& MessageList)
	{
		TArray<TSharedPtr<FJsonValue>> ValueArray;

		for (int i = 0; i < MessageList.size(); i++)
		{
			auto ItemMessagePtr = MessageList[i];
			ValueArray.Add(USIOMessageConvert::ToJsonValue(ItemMessagePtr));
		}

		SafeCallback(ValueArray);
	}, Namespace);
}

void FSIOClient::EmitNative(const FString& EventName, const TSharedPtr<FJsonObject>& ObjectMessage /*= nullptr*/, TFunction< void(const TArray<TSharedPtr<FJsonValue>>&)> CallbackFunction /*= nullptr*/, const FString& Namespace /*= FString(TEXT("/"))*/)
{
	EmitNative(EventName, MakeShareable(new FJsonValueObject(ObjectMessage)), CallbackFunction, Namespace);
}

void FSIOClient::EmitNative(const FString& EventName, const FString& StringMessage /*= FString()*/, TFunction< void(const TArray<TSharedPtr<FJsonValue>>&)> CallbackFunction /*= nullptr*/, const FString& Namespace /*= FString(TEXT("/"))*/)
{
	EmitNative(EventName, MakeShareable(new FJsonValueString(StringMessage)), CallbackFunction, Namespace);
}

void FSIOClient::EmitNative(const FString& EventName, double NumberMessage, TFunction< void(const TArray<TSharedPtr<FJsonValue>>&)> CallbackFunction /*= nullptr*/, const FString& Namespace /*= FString(TEXT("/"))*/)
{
	EmitNative(EventName, MakeShareable(new FJsonValueNumber(NumberMessage)), CallbackFunction, Namespace);
}

void FSIOClient::EmitNative(const FString& EventName, const TArray<uint8>& BinaryMessage, TFunction< void(const TArray<TSharedPtr<FJsonValue>>&)> CallbackFunction /*= nullptr*/, const FString& Namespace /*= FString(TEXT("/"))*/)
{
	EmitNative(EventName, MakeShareable(new FJsonValueBinary(BinaryMessage)), CallbackFunction, Namespace);
}

void FSIOClient::EmitNative(const FString& EventName, const TArray<TSharedPtr<FJsonValue>>& ArrayMessage, TFunction< void(const TArray<TSharedPtr<FJsonValue>>&)> CallbackFunction /*= nullptr*/, const FString& Namespace /*= FString(TEXT("/"))*/)
{
	EmitNative(EventName, MakeShareable(new FJsonValueArray(ArrayMessage)), CallbackFunction, Namespace);
}

void FSIOClient::EmitNative(const FString& EventName, bool BooleanMessage, TFunction< void(const TArray<TSharedPtr<FJsonValue>>&)> CallbackFunction /*= nullptr*/, const FString& Namespace /*= FString(TEXT("/"))*/)
{
	EmitNative(EventName, MakeShareable(new FJsonValueBoolean(BooleanMessage)), CallbackFunction, Namespace);
}

void FSIOClient::EmitNative(const FString& EventName, UStruct* Struct, const void* StructPtr, TFunction< void(const TArray<TSharedPtr<FJsonValue>>&)> CallbackFunction /*= nullptr*/, const FString& Namespace /*= FString(TEXT("/"))*/)
{
	EmitNative(EventName, USIOJConvert::ToJsonObject(Struct, (void*)StructPtr), CallbackFunction, Namespace);
}

void FSIOClient::EmitRaw(const FString& EventName, const sio::message::list& MessageList, TFunction<void(const sio::message::list&)> ResponseFunction, const FString& Namespace)
{
	const TFunction<void(const sio::message::list&)> SafeFunction = ResponseFunction;

	PrivateClient.socket(USIOMessageConvert::StdString(Namespace))->emit(
		USIOMessageConvert::StdString(EventName),
		MessageList,
		[&, SafeFunction](const sio::message::list& response)
	{
		if (SafeFunction != nullptr)
		{
			//Callback on game thread
			FFunctionGraphTask::CreateAndDispatchWhenReady([&, SafeFunction, response]
			{
				SafeFunction(response);
			}, TStatId(), nullptr, ENamedThreads::GameThread);
		}
	});
}

void FSIOClient::EmitRawBinary(const FString& EventName, uint8* Data, int32 DataLength, const FString& Namespace /*= FString(TEXT("/"))*/)
{
	PrivateClient.socket(USIOMessageConvert::StdString(Namespace))->emit(USIOMessageConvert::StdString(EventName), std::make_shared<std::string>((char*)Data, DataLength));
}

#pragma endregion Emit


#pragma region OnEvents

void FSIOClient::BindEvent(const FString& EventName, const FString& Namespace)
{
	OnRawEvent(EventName, [&](const FString& Event, const sio::message::ptr& RawMessage) {
		USIOJsonValue* NewValue = NewObject<USIOJsonValue>();
		auto Value = USIOMessageConvert::ToJsonValue(RawMessage);
		NewValue->SetRootValue(Value);
		On.ExecuteIfBound(Event, NewValue);

	}, Namespace);
}

void FSIOClient::OnNativeEvent(const FString& EventName, TFunction< void(const FString&, const TSharedPtr<FJsonValue>&)> CallbackFunction, const FString& Namespace /*= FString(TEXT("/"))*/)
{
	OnRawEvent(EventName, [&, CallbackFunction](const FString& Event, const sio::message::ptr& RawMessage) {
		CallbackFunction(Event, USIOMessageConvert::ToJsonValue(RawMessage));
	}, Namespace);
}

void FSIOClient::OnRawEvent(const FString& EventName, TFunction< void(const FString&, const sio::message::ptr&)> CallbackFunction, const FString& Namespace /*= FString(TEXT("/"))*/)
{
	const TFunction< void(const FString&, const sio::message::ptr&)> SafeFunction = CallbackFunction;	//copy the function so it remains in context

	PrivateClient.socket(USIOMessageConvert::StdString(Namespace))->on(
		USIOMessageConvert::StdString(EventName),
		sio::socket::event_listener_aux(
		[&, SafeFunction](std::string const& name, sio::message::ptr const& data, bool isAck, sio::message::list &ack_resp)
	{
		const FString SafeName = USIOMessageConvert::FStringFromStd(name);

		FFunctionGraphTask::CreateAndDispatchWhenReady([&, SafeFunction, SafeName, data]
		{
			SafeFunction(SafeName, data);
		}, TStatId(), nullptr, ENamedThreads::GameThread);
	}));
}


void FSIOClient::OnBinaryEvent(const FString& EventName, TFunction< void(const FString&, const TArray<uint8>&)> CallbackFunction, const FString& Namespace /*= FString(TEXT("/"))*/)
{
	const TFunction< void(const FString&, const TArray<uint8>&)> SafeFunction = CallbackFunction;	//copy the function so it remains in context

	PrivateClient.socket(USIOMessageConvert::StdString(Namespace))->on(
		USIOMessageConvert::StdString(EventName),
		sio::socket::event_listener_aux(
		[&, SafeFunction](std::string const& name, sio::message::ptr const& data, bool isAck, sio::message::list &ack_resp)
	{
		const FString SafeName = USIOMessageConvert::FStringFromStd(name);

		//Construct raw buffer
		if (data->get_flag() == sio::message::flag_binary)
		{
			TArray<uint8> Buffer;
			int32 BufferSize = data->get_binary()->size();
			auto MessageBuffer = data->get_binary();
			Buffer.Append((uint8*)(MessageBuffer->data()), BufferSize);

			FFunctionGraphTask::CreateAndDispatchWhenReady([&, SafeFunction, SafeName, Buffer]
			{
				SafeFunction(SafeName, Buffer);
			}, TStatId(), nullptr, ENamedThreads::GameThread);
		}
		else
		{
			UE_LOG(SocketIOLog, Warning, TEXT("Non-binary message received to binary message lambda, check server message data!"));
		}
	}));
}

#pragma endregion OnEvents