// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LoginWidget.generated.h"

/**
 * 
 */
UCLASS()
class FPS_PROJECT_API ULoginWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeOnInitialized() override;

	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

protected:
	UPROPERTY(meta = (BindWidget))
	class UWidgetSwitcher* WidgetSwitcher_Sgin;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UButton* Button_Close;

	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* TextBox_ID;

	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* TextBox_Password;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UButton* Button_Signup;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UButton* Button_FindPwd;

	UPROPERTY(meta = (BindWidget))
	class UButton* Button_StartGame;

	UPROPERTY(meta = (BindWidget))
	class UButton* Button_GuestStartgame;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TextBlock_Info;

	UPROPERTY(meta = (BindWidget))
	class USignupWidget* WBP_Signup;

	UPROPERTY(meta = (BindWidget))
	class UFindPasswordWidget* WBP_FindPassword;

protected:
	UFUNCTION()
	void Button_Close_Clicked();

	UFUNCTION()
	void Button_Signup_Clicked();

	UFUNCTION()
	void Button_FindPwd_Clicked();

	UFUNCTION()
	void Button_StartGame_Clicked();

	UFUNCTION()
	void Button_GuestStartgame_Clicked();

public:
	void CloseSignupWidget();

	UFUNCTION(BlueprintImplementableEvent)
	void ShowSuccessPopup(const FString& PopupMessage);

	void CloseFindPasswordWidget();

protected:
	void CleanComponents();

protected:
	UFUNCTION()
	void OnRecvPacket(const EPacketType& PacketType, const FString& Payload);

protected:
	UPROPERTY()
	class UTCPSubsystem* TCPSubsystem;

	FDelegateHandle RecvPacketHandle;

};
