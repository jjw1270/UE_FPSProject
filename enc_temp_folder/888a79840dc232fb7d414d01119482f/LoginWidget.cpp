// Fill out your copyright notice in the Description page of Project Settings.


#include "Login/LoginWidget.h"
#include "MyUtility.h"
#include "FPSProjectGameInstance.h"
#include "TCPSubsystem.h"

#include "SignupWidget.h"
#include "FindPasswordWidget.h"

#include "Components/WidgetSwitcher.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"

void ULoginWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	WidgetSwitcher_Sgin->SetActiveWidgetIndex(0);

	Button_Close->OnClicked.AddDynamic(this, &ULoginWidget::Button_Close_Clicked);
	Button_Signup->OnClicked.AddDynamic(this, &ULoginWidget::Button_Signup_Clicked);
	Button_FindPwd->OnClicked.AddDynamic(this, &ULoginWidget::Button_FindPwd_Clicked);
	Button_StartGame->OnClicked.AddDynamic(this, &ULoginWidget::Button_StartGame_Clicked);

	WBP_Signup->LoginWidget = this;
	WBP_FindPassword->LoginWidget = this;
}

void ULoginWidget::NativeConstruct()
{
	Super::NativeConstruct();

	TCPSubsystem = GetGameInstance()->GetSubsystem<UTCPSubsystem>();
	if (TCPSubsystem)
	{
		RecvPacketHandle = TCPSubsystem->RecvPacketDelegate.AddUObject(this, &ULoginWidget::OnRecvPacket);
	}
}

void ULoginWidget::NativeDestruct()
{
	Super::NativeDestruct();

	TCPSubsystem->RecvPacketDelegate.Remove(RecvPacketHandle);
}

void ULoginWidget::Button_Close_Clicked()
{
	WidgetSwitcher_Sgin->SetActiveWidgetIndex(0);

	CleanComponents();
	WBP_Signup->CleanComponents();
	WBP_FindPassword->CleanComponents();

	this->SetVisibility(ESlateVisibility::Collapsed);
}

void ULoginWidget::Button_Signup_Clicked()
{
	CleanComponents();
	WidgetSwitcher_Sgin->SetActiveWidgetIndex(1);
}

void ULoginWidget::Button_FindPwd_Clicked()
{
	CleanComponents();
	WidgetSwitcher_Sgin->SetActiveWidgetIndex(2);
}

void ULoginWidget::Button_StartGame_Clicked()
{
	FString ID = TextBox_ID->GetText().ToString();
	FString Pwd = TextBox_Password->GetText().ToString();

	if (ID.IsEmpty() || Pwd.IsEmpty())
	{
		TextBlock_Info->SetText(FText::FromString(TEXT("ID 또는 비밀번호를 입력해주세요.")));
		return;
	}

	TextBlock_Info->SetText(FText::FromString(TEXT("정보 확인중..")));

	//check ID, Pwd from tcp login server
	if (TCPSubsystem)
	{
		// Payload = "ID:Password"
		FString UserConfig = ID + ":" + Pwd;

		if (!TCPSubsystem->SendToLoginServer(FPacketData(EPacketType::C2S_ReqSignIn, UserConfig)))
		{
			TextBlock_Info->SetText(FText::FromString(TEXT("서버 연결 실패")));
			return;
		}
		// now wait for recv
	}
}

void ULoginWidget::CloseSignupWidget()
{
	WidgetSwitcher_Sgin->SetActiveWidgetIndex(0);
	WBP_Signup->CleanComponents();
}

void ULoginWidget::CloseFindPasswordWidget()
{
	WidgetSwitcher_Sgin->SetActiveWidgetIndex(0);
	WBP_FindPassword->CleanComponents();
}

void ULoginWidget::CleanComponents()
{
	TextBox_ID->SetText(FText::GetEmpty());
	TextBox_Password->SetText(FText::GetEmpty());

	TextBlock_Info->SetText(FText::GetEmpty());
}

void ULoginWidget::OnRecvPacket(const EPacketType& PacketType, const FString& Payload)
{
	switch (PacketType)
	{
	case EPacketType::S2C_ResSignIn_Success:
	{
		UFPSProjectGameInstance* GI = GetGameInstance<UFPSProjectGameInstance>();
		CHECK_VALID(GI);
		GI->SetUserID(Payload);
		ShowSuccessPopup(TEXT("로그인 성공!"));
		UMyUtility::WaitForSec(3.f);
		UGameplayStatics::OpenLevel(GetWorld(), TEXT("LobbyLevel"));
	}
		break;
	case EPacketType::S2C_ResSignIn_Fail_InValidID:
		TextBlock_Info->SetText(FText::FromString(TEXT("ID가 존재하지 않습니다.")));
		TextBox_ID->SetText(FText::GetEmpty());
		TextBox_Password->SetText(FText::GetEmpty());
		break;
	case EPacketType::S2C_ResSignIn_Fail_InValidPassword:
		TextBlock_Info->SetText(FText::FromString(TEXT("비밀번호가 틀립니다.")));
		TextBox_Password->SetText(FText::GetEmpty());
		break;
	case EPacketType::S2C_ResSignIn_Fail_AlreadySignIn:
		TextBlock_Info->SetText(FText::FromString(TEXT("중복된 로그인입니다!")));
		TextBox_ID->SetText(FText::GetEmpty());
		TextBox_Password->SetText(FText::GetEmpty());
		break;
	default:
		break;
	}
}
