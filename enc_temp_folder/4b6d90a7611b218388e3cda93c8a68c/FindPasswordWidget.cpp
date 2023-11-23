// Fill out your copyright notice in the Description page of Project Settings.


#include "Login/FindPasswordWidget.h"
#include "FPS_Project.h"
#include "TCPSubsystem.h"
#include "LoginWidget.h"

#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"

void UFindPasswordWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	Button_IDName->OnClicked.AddDynamic(this, &UFindPasswordWidget::Button_IDName_Clicked);
	Button_Password->OnClicked.AddDynamic(this, &UFindPasswordWidget::Button_Password_Clicked);
}

void UFindPasswordWidget::NativeConstruct()
{
	Super::NativeConstruct();

	TCPSubsystem = GetGameInstance()->GetSubsystem<UTCPSubsystem>();
	if (TCPSubsystem)
	{
		RecvPacketHandle = TCPSubsystem->RecvPacketDelegate.AddUObject(this, &UFindPasswordWidget::OnRecvPacket);
	}
}

void UFindPasswordWidget::NativeDestruct()
{
	Super::NativeDestruct();

	TCPSubsystem->RecvPacketDelegate.Remove(RecvPacketHandle);
}

void UFindPasswordWidget::Button_IDName_Clicked()
{
	FString ID = TextBox_ID->GetText().ToString();
	FString Username = TextBox_Username->GetText().ToString();

	if (TCPSubsystem)
	{
		TextBlock_Info->SetText(FText::FromString(TEXT("정보 확인중..")));

		// Payload = "ID:UserName"
		FString UserIDName = ID + ":" + Username;

		if (!TCPSubsystem->SendToLoginServer(FPacketData(EPacketType::C2S_ReqCheckIDUserName, UserIDName)))
		{
			TextBlock_Info->SetText(FText::FromString(TEXT("서버 연결 실패")));
			return;
		}
		// now wait for recv
	}
}

void UFindPasswordWidget::Button_Password_Clicked()
{
	FString NewPwd = TextBox_Password->GetText().ToString();
	FString NewPwd2 = TextBox_Password2->GetText().ToString();

	if (!CheckPwd(NewPwd, NewPwd2))
	{
		TextBox_Password->SetText(FText::GetEmpty());
		TextBox_Password2->SetText(FText::GetEmpty());
		return;
	}

	if (TCPSubsystem)
	{
		TextBlock_Info->SetText(FText::FromString(TEXT("비밀번호 변경 요청중..")));

		// Payload = "ID:Password"
		FString UserIDPassword = TextBox_ID->GetText().ToString() + ":" + NewPwd;

		if (!TCPSubsystem->SendToLoginServer(FPacketData(EPacketType::C2S_ReqNewPassword, UserIDPassword)))
		{
			TextBlock_Info->SetText(FText::FromString(TEXT("서버 연결 실패")));
			return;
		}
		// now wait for recv
	}
}

void UFindPasswordWidget::CleanComponents()
{
	TextBlock_Info->SetText(FText::GetEmpty());

	TextBox_ID->SetText(FText::GetEmpty());
	TextBox_Username->SetText(FText::GetEmpty());
	TextBox_Password->SetText(FText::GetEmpty());
	TextBox_Password2->SetText(FText::GetEmpty());

	VerticalBox_Password->SetVisibility(ESlateVisibility::Collapsed);
}

void UFindPasswordWidget::OnRecvPacket(const EPacketType& PacketType, const FString& Payload)
{
	switch (PacketType)
	{
	case EPacketType::S2C_ResCheckIDUserName_Success:
		TextBlock_Info->SetText(FText::FromString(TEXT("새로운 비밀번호를 입력하세요.")));
		VerticalBox_Password->SetVisibility(ESlateVisibility::Visible);
		break;
	case EPacketType::S2C_ResCheckIDUserName_Fail:
		TextBlock_Info->SetText(FText::FromString(TEXT("ID와 닉네임이 일치하지 않습니다.")));
		TextBox_ID->SetText(FText::GetEmpty());
		TextBox_Username->SetText(FText::GetEmpty());
		break;
	case EPacketType::S2C_ResCheckIDUserName_Fail_IDNotExist:
		TextBlock_Info->SetText(FText::FromString(TEXT("ID가 존재하지 않습니다.")));
		TextBox_ID->SetText(FText::GetEmpty());
		TextBox_Username->SetText(FText::GetEmpty());
		break;
	case EPacketType::S2C_ResNewPassword:
		LoginWidget->CloseFindPasswordWidget();
		LoginWidget->ShowSuccessPopup(TEXT("비밀번호가 변경되었습니다!"));
		break;
	default:
		break;
	}
}

bool UFindPasswordWidget::CheckPwd(const FString& NewPwd, const FString& NewPwd2)
{
	// Check verify pwd
	if (NewPwd != NewPwd2)
	{
		TextBlock_Info->SetText(FText::FromString(TEXT("비밀번호를 확인하세요.")));
		return false;
	}

	//Pwd : 8~20, Eng/num/symbol(!,@,#,$,%)
	if (NewPwd.Len() < 8 && NewPwd.Len() > 20)
	{
		TextBlock_Info->SetText(FText::FromString(TEXT("비밀번호는 8~20자 내외입니다.")));
		return false;
	}

	for (const auto& Ch : NewPwd)
	{
		// Check is Eng/num/(!,@,#,$,%)
		if (!((Ch >= 'a' && Ch <= 'z') || (Ch >= 'A' && Ch <= 'Z') ||
			(Ch >= '0' && Ch <= '9') ||
			(Ch == '!' || Ch == '@' || Ch == '#' || Ch == '$' || Ch == '%')))
		{
			TextBlock_Info->SetText(FText::FromString(TEXT("비밀번호는 영문,숫자,!,@,#,$,% 조합만 가능합니다.")));
			return false;
		}
	}

	return true;
}
