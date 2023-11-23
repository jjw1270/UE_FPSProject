// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FindPasswordWidget.generated.h"

/**
 * 
 */
UCLASS()
class FPS_PROJECT_API UFindPasswordWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY()
	class ULoginWidget* LoginWidget;

protected:
	virtual void NativeOnInitialized() override;

	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

protected:
	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* TextBox_ID;

	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* TextBox_Username;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UButton* Button_IDName;

	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* TextBox_Password;

	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* TextBox_Password2;

	UPROPERTY(meta = (BindWidget))
	class UButton* Button_Password;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TextBlock_Info;

	UPROPERTY(meta = (BindWidget))
	class UVerticalBox* VerticalBox_Password;
	
protected:
	UFUNCTION()
	void Button_IDName_Clicked();

	UFUNCTION()
	void Button_Password_Clicked();

public:
	void CleanComponents();

protected:
	UFUNCTION()
	void OnRecvPacket(const EPacketType& PacketType, const FString& Payload);

protected:
	UPROPERTY()
	class UTCPSubsystem* TCPSubsystem;

	FDelegateHandle RecvPacketHandle;

protected:
	bool CheckPwd(const FString& NewPwd, const FString& NewPwd2);

};
