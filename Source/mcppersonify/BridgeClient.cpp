#include "BridgeClient.h"
#include "WebSocketsModule.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"

UBridgeClient::UBridgeClient()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UBridgeClient::BeginPlay()
{
	Super::BeginPlay();
	FModuleManager::Get().LoadModuleChecked(TEXT("WebSockets"));
}

void UBridgeClient::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Disconnect();
	Super::EndPlay(EndPlayReason);
}

void UBridgeClient::Connect()
{
	if (WebSocket.IsValid()) return;

	const FString Url = FString::Printf(TEXT("ws://%s:%d"), *Host, Port);
	UE_LOG(LogTemp, Log, TEXT("[BridgeClient] Connecting to %s"), *Url);

	WebSocket = FWebSocketsModule::Get().CreateWebSocket(Url, TEXT("ws"));

	WebSocket->OnConnected().AddLambda([this]()
	{
		bConnected = true;
		UE_LOG(LogTemp, Log, TEXT("[BridgeClient] Connected"));
		OnConnected.Broadcast();
	});

	WebSocket->OnConnectionError().AddLambda([this](const FString& Error)
	{
		UE_LOG(LogTemp, Warning, TEXT("[BridgeClient] Connection error: %s"), *Error);
		OnError.Broadcast(Error);
	});

	WebSocket->OnClosed().AddLambda([this](int32 StatusCode, const FString& Reason, bool bWasClean)
	{
		bConnected = false;
		UE_LOG(LogTemp, Log, TEXT("[BridgeClient] Disconnected (%d: %s)"), StatusCode, *Reason);
		OnDisconnected.Broadcast();
	});

	WebSocket->OnMessage().AddLambda([this](const FString& Message)
	{
		HandleMessage(Message);
	});

	WebSocket->Connect();
}

void UBridgeClient::Disconnect()
{
	if (WebSocket.IsValid())
	{
		WebSocket->Close();
		WebSocket.Reset();
		bConnected = false;
	}
}

void UBridgeClient::SendSpeak(const FString& Text, const FString& Voice,
                              const FString& Format, const FString& Emotion, float Intensity)
{
	TSharedPtr<FJsonObject> Obj = MakeShared<FJsonObject>();
	Obj->SetStringField(TEXT("type"), TEXT("speak"));
	Obj->SetStringField(TEXT("text"), Text);
	Obj->SetStringField(TEXT("voice"), Voice);
	Obj->SetStringField(TEXT("format"), Format);
	Obj->SetStringField(TEXT("emotion"), Emotion);
	Obj->SetNumberField(TEXT("intensity"), Intensity);

	FString Json;
	auto Writer = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&Json);
	FJsonSerializer::Serialize(Obj.ToSharedRef(), Writer);
	Send(Json);
}

void UBridgeClient::SendStatus()
{
	Send(TEXT("{\"type\":\"status\"}"));
}

void UBridgeClient::SendStop()
{
	Send(TEXT("{\"type\":\"stop\"}"));
}

void UBridgeClient::Send(const FString& Json)
{
	if (!WebSocket.IsValid() || !bConnected) return;
	WebSocket->Send(Json);
}

void UBridgeClient::HandleMessage(const FString& Message)
{
	TSharedPtr<FJsonObject> JsonObj;
	auto Reader = TJsonReaderFactory<>::Create(Message);
	if (!FJsonSerializer::Deserialize(Reader, JsonObj) || !JsonObj.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("[BridgeClient] Failed to parse: %s"), *Message);
		return;
	}

	FString Type;
	if (!JsonObj->TryGetStringField(TEXT("type"), Type)) return;

	if (Type == TEXT("tts.play"))
	{
		FTtsPlayMessage Msg;
		JsonObj->TryGetStringField(TEXT("path"), Msg.Path);
		JsonObj->TryGetStringField(TEXT("voice"), Msg.Voice);
		JsonObj->TryGetStringField(TEXT("text"), Msg.Text);
		JsonObj->TryGetNumberField(TEXT("durationMs"), Msg.DurationMs);
		JsonObj->TryGetStringField(TEXT("format"), Msg.Format);
		JsonObj->TryGetNumberField(TEXT("sampleRate"), Msg.SampleRate);
		JsonObj->TryGetStringField(TEXT("emotion"), Msg.Emotion);
		JsonObj->TryGetNumberField(TEXT("intensity"), Msg.Intensity);
		OnTtsPlay.Broadcast(Msg);
	}
	else if (Type == TEXT("tts.status"))
	{
		TArray<FVoiceInfo> Voices;
		const TArray<TSharedPtr<FJsonValue>>* VoicesArray;
		if (JsonObj->TryGetArrayField(TEXT("voices"), VoicesArray))
		{
			for (auto& Val : *VoicesArray)
			{
				auto VObj = Val->AsObject();
				if (!VObj.IsValid()) continue;
				FVoiceInfo V;
				VObj->TryGetStringField(TEXT("id"), V.Id);
				VObj->TryGetStringField(TEXT("name"), V.Name);
				VObj->TryGetStringField(TEXT("lang"), V.Lang);
				VObj->TryGetStringField(TEXT("gender"), V.Gender);
				Voices.Add(V);
			}
		}
		OnVoicesList.Broadcast(Voices);
	}
	else if (Type == TEXT("tts.error"))
	{
		FString ErrMsg;
		JsonObj->TryGetStringField(TEXT("message"), ErrMsg);
		UE_LOG(LogTemp, Warning, TEXT("[BridgeClient] TTS error: %s"), *ErrMsg);
		OnError.Broadcast(ErrMsg);
	}
	else if (Type == TEXT("aside.item"))
	{
		const TSharedPtr<FJsonObject>* MetaObj;
		if (JsonObj->TryGetObjectField(TEXT("meta"), MetaObj))
		{
			FAsideCue Cue;
			(*MetaObj)->TryGetStringField(TEXT("emotion"), Cue.Emotion);
			(*MetaObj)->TryGetNumberField(TEXT("intensity"), Cue.Intensity);
			OnAsideCue.Broadcast(Cue);
		}
	}
}
