// Fill out your copyright notice in the Description page of Project Settings.


#include "GameServerActor.h"


using namespace std::chrono_literals;
// Sets default values
AGameServerActor::AGameServerActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

 
void AGameServerActor::sendToClient(UJSONComponent* jo)
{
	if(serverRequestQueue == nullptr)
	{
		serverRequestQueue = new std::queue<UJSONComponent*>();
	}

	serverRequestQueue->push(jo);
}
// Called when the game starts or when spawned
void AGameServerActor::BeginPlay()
{
	Super::BeginPlay();

	int major, minor, patch;
	zmq::version(&major, &minor, &patch);
	UE_LOG(LogTemp, Warning, TEXT("ZeroMQ version: v%d.%d.%d"), major, minor, patch);
 
	simulateServer();
} 
void AGameServerActor::server()
{
	// initialize the zmq context with a single IO thread 
	// construct a REP (reply) socket and bind to interface
	socket = zmq::socket_t(context, zmq::socket_type::rep);
	socket.bind("tcp://*:5555");



	// prepare some static data for responses
	 while(socket != nullptr && socket.connected())
	 {
		UE_LOG(LogTemp, Log, TEXT("Waiting for request:  "));
		zmq::message_t request;
	 
		// receive a request from client 
		socket.recv(request, zmq::recv_flags::none);

		FString rqst(request.to_string().c_str()); 

	 	std::string s = request.to_string();

	 	if(s[0] == '{')
	 	{ 
	 		//JSON
	 		json j = json::parse(s);
	 		
	 		if(j.contains("value"))
	 		{

	 			/*
	 			using variant = TVariant<int,float,std::string,bool>;

	 			auto v = variant{};
	 			
	 			auto valueType = j["value"].type();
	 			auto value = j["value"];
				if(valueType == json::value_t::boolean)
				{
					v = variant{int};
				} */
	 			auto jso = NewObject<UJSONComponent>();
	 			jso->construct(j);
	 			//jso.FromJSON(j);
	 			FString fs(jso->toString().c_str());
	 			UE_LOG(LogTemp, Log, TEXT("Recieved JSON:  %s"), *fs);

	 			//Parcours dans le dictionnaire pour check si l'id est a un endroit
	 			attach(jso);
	 			//DEBUG envoie la donnée en retour au client
	 			//sendToClient(jso);
	 			jso->DestroyComponent();
	 		}else
	 		{
	 			
	 			UE_LOG(LogTemp, Log, TEXT("Recieved JSON but couldn't parse it"));
	 		}
	 	}else
	 	{
	 		
	 		UE_LOG(LogTemp, Log, TEXT("Recieved:  %s"), *rqst);
	 	}
	 		
		// simulate work
		//std::this_thread::sleep_for(1s);
		
		// send the reply to the client 
		socket.send(zmq::buffer(""), zmq::send_flags::dontwait);


	 }
}

void AGameServerActor::attach(UJSONComponent* jo)
{ 
 
	if(JSONObjects == nullptr)
	{
		JSONObjects = new std::map<FString,UJSONComponent*>();
	} 
	auto   it = JSONObjects->find(jo->id);
	if (it != JSONObjects->end())
	{
		it->second->notify(jo);
	}else
	{
		if(jo != NULL)
		{ 
			JSONObjects->insert({jo->id,jo});
		}
	}
 
}

void AGameServerActor::sendRequests()
{
	// initialize the zmq context with a single IO thread
	 
	//Socket pour envoyer des requêtes au client
	socketServer = zmq::socket_t(context, zmq::socket_type::req);
	try
	{
	socketServer.connect( "tcp://localhost:5558" );
 
	}catch (const std::exception &exc)
	{ 
		FString fs(exc.what());
		UE_LOG(LogTemp, Log, TEXT("Send JSON:  %s"), *fs);
	}
	
	while(socket != nullptr && socketServer.connected())
	{
		while(serverRequestQueue == nullptr || serverRequestQueue->empty() )
		{
			//a tester
			FPlatformProcess::Sleep(1);
			
		}
		json js = serverRequestQueue->front()->toJSON();
		
		
		FString fs(js.dump().c_str());
		UE_LOG(LogTemp, Log, TEXT("Send JSON:  %s"), *fs);
		
		socketServer.send(zmq::buffer(js.dump()), zmq::send_flags::dontwait);

		UE_LOG(LogTemp, Log, TEXT("Sent:  %s"), *fs);
		
		serverRequestQueue->pop();
		zmq::message_t request;
		UE_LOG(LogTemp, Log, TEXT("Waiting for answer "));
		// receive a request from client 
		socketServer.recv(request, zmq::recv_flags::none);
		UE_LOG(LogTemp, Log, TEXT("Answer recieved "));
		//a tester
		FPlatformProcess::Sleep(.01);
	}
	
}
void AGameServerActor::simulateServer()
{ 
	auto ptr = this;
	auto ProcessServer = Async(EAsyncExecution::Thread, [&ptr]() { ptr->server(); });
	
	auto RequestSender = Async(EAsyncExecution::Thread, [&ptr]() { ptr->sendRequests(); });
	
}

// Called every frame
void AGameServerActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGameServerActor::Destroyed()
{
	socket.close();
	socketServer.close();
	UE_LOG(LogTemp, Warning, TEXT("Desstroying"));
 
	Super::Destroyed();
}