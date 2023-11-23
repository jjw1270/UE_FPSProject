// Fill out your copyright notice in the Description page of Project Settings.


#include "Login/LoginWidget.h"
#include "Components/WidgetSwitcher.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"

#include "SignupWidget.h"
#include "FindPasswordWidget.h"

void ULoginWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	WidgetSwitcher_Sgin->SetActiveWidgetIndex(0);

	Button_Close->OnClicked.AddDynamic(this, &ULoginWidget::Button_Close_Clicked);
	Button_Signup->OnClicked.AddDynamic(this, &ULoginWidget::Button_Signup_Clicked);
	Button_FindPwd->OnClicked.AddDynamic(this, &ULoginWidget::Button_FindPwd_Clicked);
	Button_StartGame->OnClicked.AddDynamic(this, &ULoginWidget::Button_StartGame_Clicked);
	Button_GuestStartgame->OnClicked.AddDynamic(this, &ULoginWidget::Button_GuestStartgame_Clicked);

	WBP_Signup->LoginWidget = this;
	WBP_FindPassword->LoginWidget = this;
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
	//check ID, Pwd from tcp login server
}

void ULoginWidget::Button_GuestStartgame_Clicked()
{
	// later! -> random guest nickname
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
}
