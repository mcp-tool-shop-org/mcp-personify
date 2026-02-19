#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "IWebSocket.h"
#include "BridgeClient.generated.h"

/** Data from a tts.play message */
USTRUCT(BlueprintType)
struct FTtsPlayMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly) FString Path;
	UPROPERTY(BlueprintReadOnly) FString Voice;
	UPROPERTY(BlueprintReadOnly) FString Text;
	UPROPERTY(BlueprintReadOnly) int32 DurationMs = 0;
	UPROPERTY(BlueprintReadOnly) FString Format;
	UPROPERTY(BlueprintReadOnly) int32 SampleRate = 24000;
	UPROPERTY(BlueprintReadOnly) FString Emotion;
	UPROPERTY(BlueprintReadOnly) float Intensity = 0.5f;
};

/** Voice info from tts.status */
USTRUCT(BlueprintType)
struct FVoiceInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly) FString Id;
	UPROPERTY(BlueprintReadOnly) FString Name;
	UPROPERTY(BlueprintReadOnly) FString Lang;
	UPROPERTY(BlueprintReadOnly) FString Gender;
};

/** Aside emotion cue */
USTRUCT(BlueprintType)
struct FAsideCue
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly) FString Emotion;
	UPROPERTY(BlueprintReadOnly) float Intensity = 0.5f;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTtsPlay, const FTtsPlayMessage&, Message);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnVoicesList, const TArray<FVoiceInfo>&, Voices);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAsideCue, const FAsideCue&, Cue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBridgeError, const FString&, ErrorMessage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBridgeConnected);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBridgeDisconnected);

/**
 * WebSocket client that speaks the TTS bridge protocol on ws://127.0.0.1:9200.
 * Attach to the PersonifyManager actor.
 */
UCLASS(ClassGroup=(Personify), meta=(BlueprintSpawnableComponent))
class MCPPERSONIFY_API UBridgeClient : public UActorComponent
{
	GENERATED_BODY()

public:
	UBridgeClient();

	// --- Config ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Bridge")
	FString Host = TEXT("127.0.0.1");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Bridge")
	int32 Port = 9200;

	// --- Events ---
	UPROPERTY(BlueprintAssignable, Category="Bridge") FOnTtsPlay OnTtsPlay;
	UPROPERTY(BlueprintAssignable, Category="Bridge") FOnVoicesList OnVoicesList;
	UPROPERTY(BlueprintAssignable, Category="Bridge") FOnAsideCue OnAsideCue;
	UPROPERTY(BlueprintAssignable, Category="Bridge") FOnBridgeError OnError;
	UPROPERTY(BlueprintAssignable, Category="Bridge") FOnBridgeConnected OnConnected;
	UPROPERTY(BlueprintAssignable, Category="Bridge") FOnBridgeDisconnected OnDisconnected;

	// --- State ---
	UFUNCTION(BlueprintPure, Category="Bridge")
	bool IsConnected() const { return bConnected; }

	// --- Commands ---
	UFUNCTION(BlueprintCallable, Category="Bridge")
	void Connect();

	UFUNCTION(BlueprintCallable, Category="Bridge")
	void Disconnect();

	UFUNCTION(BlueprintCallable, Category="Bridge")
	void SendSpeak(const FString& Text, const FString& Voice = TEXT("am_fenrir"),
	               const FString& Format = TEXT("ogg"), const FString& Emotion = TEXT(""),
	               float Intensity = 0.5f);

	UFUNCTION(BlueprintCallable, Category="Bridge")
	void SendStatus();

	UFUNCTION(BlueprintCallable, Category="Bridge")
	void SendStop();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	TSharedPtr<IWebSocket> WebSocket;
	bool bConnected = false;

	void HandleMessage(const FString& Message);
	void Send(const FString& Json);
};
