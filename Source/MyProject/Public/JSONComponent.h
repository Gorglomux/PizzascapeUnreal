// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <json.hpp>
#include <string>
using json = nlohmann::json;
 
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "JSONComponent.generated.h"
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnValueChanged);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYPROJECT_API UJSONComponent : public UActorComponent 
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UJSONComponent();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="JSON Infos")
	FString id;
	
	TVariant<int,float,std::string,bool> value;
	
	UPROPERTY(BlueprintAssignable, Category="Custom")
	FOnValueChanged OnValueChanged;


	DECLARE_EVENT(JSONValueChanged, FOnValueChanged)
	UJSONComponent(json j)
	{
		if(j.contains("id"))
		{
			id = j["id"].dump().c_str();
		}
		if(j.contains("value"))
		{
			auto type = j["value"].type();
			if(type == json::value_t::boolean)
			{ 
				value.Set<bool>(j["value"]); 
			}else if(type == json::value_t::number_float)
			{ 
				value.Set<float>(j["value"]); 
			}else if(type  == json::value_t::number_unsigned || type  == json::value_t::number_integer)
			{ 
				value.Set<int>(j["value"]); 
			}else if(type  == json::value_t::string)
			{ 
				value.Set<std::string>(j["value"]);
			}
		}
	}

	void construct(json j)
	{
		if(j.contains("id"))
		{
			id = j["id"].dump().c_str();
		}
		if(j.contains("value"))
		{
			auto type = j["value"].type();
			if(type == json::value_t::boolean)
			{ 
				value.Set<bool>(j["value"]); 
			}else if(type == json::value_t::number_float)
			{ 
				value.Set<float>(j["value"]); 
			}else if(type  == json::value_t::number_unsigned || type  == json::value_t::number_integer)
			{ 
				value.Set<int>(j["value"]); 
			}else if(type  == json::value_t::string)
			{ 
				value.Set<std::string>(j["value"]);
			}
		}	
	}
	/*
	UJSONComponent (const UJSONComponent& jo)
	{
		this->value = jo.value;
		this->id = jo.id;
	}*/
	json toJSON()
	{
		json j;
		std::string const s = TCHAR_TO_UTF8(*id);
		
		j["id"]=s;
			
		if(value.IsType<int>())
		{ 
			j["value"] = value.Get<int>(); 
			j["type"] ="int"; 
		}else if(value.IsType<float>())
		{ 
			j["value"] = value.Get<float>();
			j["type"] ="float";
		}else if(value.IsType<bool>())
		{ 
			j["value"] = value.Get<bool>();
			j["type"] ="bool";
		}else if(value.IsType<std::string>())
		{ 
			j["value"] = value.Get<std::string>();
			j["type"] ="string";
		}
			
		return j;
	}
	std::string toString ()
	{
		std::string outValue = "";
		if(value.IsType<int>())
		{ 
			outValue =std::to_string(value.Get<int>()); 
		}else if(value.IsType<float>())
		{
			outValue =std::to_string(value.Get<float>()); 
		}else if(value.IsType<bool>())
		{ 
			outValue = value.Get<bool>() ? "true" : "false";
		}else if(value.IsType<std::string>())
		{
			outValue =  value.Get<std::string>();
		}
		return "id : " + std::string(TCHAR_TO_UTF8(*id)) + ", value " + outValue+"\n";  
	}
	~UJSONComponent()
	{
	} 
	void send()
	{
		
	}
	void notify(UJSONComponent* jo)
	{ 
		UE_LOG(LogTemp, Log, TEXT("NOtified"));
		value = jo->value;
		OnValueChanged.Broadcast();
		
	}

	UFUNCTION(BlueprintImplementableEvent, category = "JSON Events")
	void valueChangedEvent();
	void valueChangedEvent_Implementation()
	{
		
	}
	UFUNCTION(BlueprintCallable, Category="JSON Value Setter")
	void setValueBool(bool v)
	{
		value.Set<bool>(v);
	}
	UFUNCTION(BlueprintCallable, Category="JSON Value Setter")
	void setValueFloat(float v)
	{
		value.Set<float>(v);
	}
	UFUNCTION(BlueprintCallable, Category="JSON Value Setter")
	void setValueInt(int v)
	{
		value.Set<int>(v);
	}
	UFUNCTION(BlueprintCallable, Category="JSON Value Setter")
	void setValueString(FString v)
		{
			
			value.Set<std::string>(TCHAR_TO_UTF8(*v));
		}
	UFUNCTION(BlueprintCallable, Category="JSON Value Getter")
	FString getValueString()
	{
		return value.Get<std::string>().c_str();
	}
	UFUNCTION(BlueprintCallable, Category="JSON Value Getter")
	float getValueFloat()
	{
		return value.Get<float>();
	}
	UFUNCTION(BlueprintCallable, Category="JSON Value Getter")
	int getValueInt()
	{
		return value.Get<int>();
	}
	UFUNCTION(BlueprintCallable, Category="JSON Value Getter")
	bool getValueBool()
	{
		return value.Get<bool>();
	}
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
