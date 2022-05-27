// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <string>
#include <chrono>
#include <thread>
#include <iostream>
 
#define WIN32_LEAN_AND_MEAN
#include <zmq.hpp> 
 
#include "Async/Async.h" 
#include <json.hpp>
#include <queue>
#include <map>
#include "JSONComponent.h"

// for convenience
using json = nlohmann::json;

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "JSONComponent.h"
#include "GameServerActor.generated.h"


UCLASS()
class MYPROJECT_API AGameServerActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGameServerActor();

	
	std::queue<UJSONComponent*>* serverRequestQueue;
	
	//A appeller avec son objet a envoyer

	UFUNCTION(BlueprintCallable, Category="Server send functions")
	void sendToClient(UJSONComponent* jo);

	void server();
	void Destroyed() override;
	void sendRequests();
	
	std::map<FString,UJSONComponent*>* JSONObjects;
	UFUNCTION(BlueprintCallable, Category="Server send functions")
	void attach(UJSONComponent* jo);

	zmq::socket_t socket;
	zmq::socket_t socketServer;
	zmq::context_t context{1};
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	void simulateServer();
	// Called every frame
	virtual void Tick(float DeltaTime) override;


};
