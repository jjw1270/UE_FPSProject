// Fill out your copyright notice in the Description page of Project Settings.


#include "Login/SignupWidget.h"

#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"

void USignupWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	Button_Signup->OnClicked.AddDynamic(this, &USignupWidget::Button_Signup_Clicked);
}

void USignupWidget::NativeConstruct()
{
	Super::NativeConstruct();

	TCPSubsystem = GetGameInstance()->GetSubsystem<UTCPSubsystem>();
	if (TCPSubsystem)
	{
		RecvPacketHandle = TCPSubsystem->RecvPacketDelegate.AddUObject(this, &USignupWidget::OnRecvPacket);
	}
}

void USignupWidget::NativeDestruct()
{
	Super::NativeDestruct();

	TCPSubsystem->RecvPacketDelegate.Remove(RecvPacketHandle);
}

void USignupWidget::Button_Signup_Clicked()
{
	// Check Password verifying
	// C2S_ReqSignUp, "ID:Password:NickName"
	// wait for recv
	// if(success) : close window with popup(blink 1sec)
	// else : set text info

	if (!TextBox_Password->GetText().CompareTo(TextBox_Password2->GetText()))
	{
		TextBlock_Info->SetText(FText::FromString(TEXT("비밀번호를 확인하세요.")));
		TextBox_Password->SetText(FText::GetEmpty());
		TextBox_Password2->SetText(FText::GetEmpty());
		return;
	}

	if (TCPSubsystem)
	{
		FString NewUserConfig = 
			TextBox_ID->GetText().ToString() + ":" +
			TextBox_Password->GetText().ToString() + ":" +
			TextBox_UserName->GetText().ToString();

		if (!TCPSubsystem->SendToLoginServer(FPacketData(EPacketType::C2S_ReqSignUp, NewUserConfig)))
		{
			TextBlock_Info->SetText(FText::FromString(TEXT("서버 연결 실패")));
			return;
		}
	}
}

void USignupWidget::CleanComponents()
{

}

void USignupWidget::OnRecvPacket(const EPacketType& PacketType, const FString& Payload)
{


}
