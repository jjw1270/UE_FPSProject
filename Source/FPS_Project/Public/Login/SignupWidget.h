// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TCPSubsystem.h"
#include "Blueprint/UserWidget.h"
#include "SignupWidget.generated.h"

/**
 * 
 */
UCLASS()
class FPS_PROJECT_API USignupWidget : public UUserWidget
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
	class UEditableTextBox* TextBox_Password;

	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* TextBox_Password2;

	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* TextBox_UserName;

	UPROPERTY(meta = (BindWidget))
	class UButton* Button_Signup;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TextBlock_Info;
	
protected:
	UFUNCTION()
	void Button_Signup_Clicked();

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
	bool CheckID(const FString& NewID);

	bool CheckPwd(const FString& NewPwd, const FString& NewPwd2);

	bool CheckUsername(const FString& NewUsername);
};
