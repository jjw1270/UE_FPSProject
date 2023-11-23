// Fill out your copyright notice in the Description page of Project Settings.


#include "Login/SignupWidget.h"
#include "FPS_Project.h"
#include "TCPSubsystem.h"
#include "LoginWidget.h"

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
	FString NewID = TextBox_ID->GetText().ToString();
	FString NewPwd = TextBox_Password->GetText().ToString();
	FString NewPwd2 = TextBox_Password2->GetText().ToString();
	FString NewUsername = TextBox_UserName->GetText().ToString();

	// Check ID, Password, Nickname
	if (!CheckID(NewID))
	{
		TextBox_ID->SetText(FText::GetEmpty());
		return;
	}

	if (!CheckPwd(NewPwd, NewPwd2))
	{
		TextBox_Password->SetText(FText::GetEmpty());
		TextBox_Password2->SetText(FText::GetEmpty());
		return;
	}

	if (!CheckUsername(NewUsername))
	{
		TextBox_UserName->SetText(FText::GetEmpty());
		return;
	}

	if (TCPSubsystem)
	{
		TextBlock_Info->SetText(FText::FromString(TEXT("정보 확인중..")));

		// Payload = "ID:Password:UserName"
		FString NewUserConfig = NewID + ":" + NewPwd + ":" + NewUsername;

		if (!TCPSubsystem->SendToLoginServer(FPacketData(EPacketType::C2S_ReqSignUp, NewUserConfig)))
		{
			TextBlock_Info->SetText(FText::FromString(TEXT("서버 연결 실패")));
			return;
		}
		// now wait for recv
	}
}

void USignupWidget::CleanComponents()
{
	TextBox_ID->SetText(FText::GetEmpty());
	TextBox_Password->SetText(FText::GetEmpty());
	TextBox_Password2->SetText(FText::GetEmpty());
	TextBox_UserName->SetText(FText::GetEmpty());

	TextBlock_Info->SetText(FText::GetEmpty());
}

void USignupWidget::OnRecvPacket(const EPacketType& PacketType, const FString& Payload)
{
	switch (PacketType)
	{
	case EPacketType::S2C_ResSignUp_Success:
		LoginWidget->CloseSignupWidget();
		LoginWidget->ShowSuccessPopup(TEXT("회원가입 성공!"));
		break;
	case EPacketType::S2C_ResSignUp_Fail_ID:
		TextBlock_Info->SetText(FText::FromString(TEXT("ID가 이미 존재합니다.")));
		TextBox_ID->SetText(FText::GetEmpty());
		break;
	case EPacketType::S2C_ResSignUp_Fail_NickName:
		TextBlock_Info->SetText(FText::FromString(TEXT("닉네임이 이미 존재합니다.")));
		TextBox_UserName->SetText(FText::GetEmpty());
		break;
	default:
		break;
	}
}

bool USignupWidget::CheckID(const FString& NewID)
{
	//ID: 4~16, Eng/num
	if (NewID.Len() >= 4 && NewID.Len() <= 12)
	{
		for (const auto& Ch : NewID)
		{
			// Check is Eng/num
			if (!((Ch >= 'a' && Ch <= 'z') || (Ch >= 'A' && Ch <= 'Z') || 
				(Ch >= '0' && Ch <= '9')))
			{
				TextBlock_Info->SetText(FText::FromString(TEXT("아이디는 4~16자의 영문,숫자 조합만 가능합니다.")));
				return false;
			}
		}
	}

	return true;
}

bool USignupWidget::CheckPwd(const FString& NewPwd, const FString& NewPwd2)
{
	// Check verify pwd
	if (NewPwd != NewPwd2)
	{
		TextBlock_Info->SetText(FText::FromString(TEXT("비밀번호를 확인하세요.")));
		return false;
	}

	//Pwd : 8~20, Eng/num/symbol(!,@,#,$,%)
	if (NewPwd.Len() >= 8 && NewPwd.Len() <= 20)
	{
		for (const auto& Ch : NewPwd)
		{
			// Check is Eng/num/(!,@,#,$,%)
			if (!((Ch >= 'a' && Ch <= 'z') || (Ch >= 'A' && Ch <= 'Z') ||
				(Ch >= '0' && Ch <= '9') ||
				(Ch == '!' || Ch == '@' || Ch == '#' || Ch == '$' || Ch == '%')))
			{
				TextBlock_Info->SetText(FText::FromString(TEXT("비밀번호는 8~20자의 영문,숫자,!,@,#,$,% 조합만 가능합니다.")));
				return false; 
			}
		}
	}

	return true;
}

bool USignupWidget::CheckUsername(const FString& NewUsername)
{
	// UserName : Eng(4~16)/Kor(2~8)/num
	int32 StrLen = 0;

	if (NewUsername.Len() >= 4 && NewUsername.Len() <= 16)
	{
		for (const auto& Ch : NewUsername)
		{
			// Check is Eng/Kor/num
			if (!((Ch >= 'a' && Ch <= 'z') || (Ch >= 'A' && Ch <= 'Z') ||
				(Ch >= 0xAC00 && Ch <= 0xD7A3) ||
				(Ch >= '0' && Ch <= '9')))
			{
				TextBlock_Info->SetText(FText::FromString(TEXT("닉네임은 4~16자의 영문 또는 2~8자의 한글과 숫자 조합만 가능합니다.")));
				return false;
			}

			if ((Ch >= 0xAC00 && Ch <= 0xD7A3))
			{
				StrLen += 2;
			}
			else
			{
				StrLen++;
			}
		}
	}

	return true;
}
