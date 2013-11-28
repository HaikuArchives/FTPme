#include "FTPmeView.h"
#include <Application.h>
#include <ctype.h>
#include <string>

FTPmeView::FTPmeView(): BView(mainViewFrame, "mainView", B_FOLLOW_LEFT|B_FOLLOW_TOP, B_WILL_DRAW)
{
	activityCounter = 999;
	
	const rgb_color BKG_GREY		= { 216, 216, 216, 0 };
	SetViewColor(BKG_GREY);
	
	username = new BTextControl(usernameFrame, B_EMPTY_STRING, "Username:", "", NULL);
	username->SetDivider(55);
	password = new PassControl(passwordFrame, B_EMPTY_STRING, "Password:", "", NULL);
	password->SetDivider(55);
	site = new BTextControl(siteFrame, B_EMPTY_STRING, "Site:", "", NULL);
	site->SetDivider(30);
	path = new BTextControl(pathFrame, B_EMPTY_STRING, "Path:", "", NULL);
	path->SetDivider(30);
	
	anonymous = new BCheckBox(anonymousFrame, B_EMPTY_STRING, "Anonymous", new BMessage(ANON_EVENT));
	log = new BCheckBox(logFrame, B_EMPTY_STRING, "Keep Log", new BMessage('poip'));

	go = new BButton(goButtonFrame, "go", "Go", new BMessage(GO_EVENT));
	xferRate = new BStringView(statusFrame, B_EMPTY_STRING, "");
	xferRate->SetAlignment(B_ALIGN_RIGHT);
	//AddChild(xferRate);
	
	strcpy(tempuser,"");
	strcpy(temppass,"");
	AddChild(username);
	AddChild(password);
	AddChild(site);
	AddChild(path);
	AddChild(anonymous);
	AddChild(log);
	AddChild(go);
	
	LoadSettings();
}

void FTPmeView::Draw(BRect updateRect) {

	if (activityCounter != 999) {				// don't draw it if the transfer hasn't begun
//		SetHighColor(180,180,184,255);	//draw the light shadow
//		StrokeRect(shadowFrame2);
//	
//		SetHighColor(129,129,133,255);	//draw the dark shadow
//		StrokeRect(shadowFrame);
	
		SetLowColor(188,197,235,255);	//light blue
		SetHighColor(66,61,87,255);			//dark grey for the stripes
		
		switch (activityCounter) {
			case 0: FillRect(progressFrame, stripes1); break;
			case 1: FillRect(progressFrame, stripes2); break;
			case 2: FillRect(progressFrame, stripes3); break;
			case 3: FillRect(progressFrame, stripes4); break;
			case 4: FillRect(progressFrame, stripes5); break;
			case 5: FillRect(progressFrame, stripes6); break;
			case 6: FillRect(progressFrame, stripes7); break;
			case 7: FillRect(progressFrame, stripes8); break;
			default:
				FillRect(progressFrame, stripes1);
		}
		
		SetHighColor(0,0,0,255);				//draw the black border
		StrokeRect(progressFrame);
	
		SetDrawingMode(B_OP_ALPHA);		//emboss the bar!
		SetHighColor(0,0,0,127);
		StrokeLine(bar_bottomleft, bar_bottomright);
		StrokeLine(bar_topright, bar_bottomright);
		SetHighColor(255,255,255,127);
		StrokeLine(bar_topleft, bar_topright);
		StrokeLine(bar_topleft, bar_bottomleft);
	}
}

const char *FTPmeView::Username() const
{
	return username->Text();
}

const char *FTPmeView::ActualPassword() const
{
	return password->actualText();
}

const char *FTPmeView::Site() const
{
	return site->Text();
}

const char *FTPmeView::Path() const
{
	return path->Text();
}

int32 FTPmeView::LogValue() const
{
	return log->Value();
}

void FTPmeView::AnonymousToggle()
{
	if (anonymous->Value() == B_CONTROL_OFF) {
		username->SetText(tempuser);
		password->SetText("*****");
		password->SetActual(temppass);
	}
	else {
		sprintf(tempuser, "%s", username->Text());				//store the old username and pass
		sprintf(temppass, "%s", password->actualText());	//incase they want to get it back
		username->SetText("anonymous");
		password->SetText("*****");
		password->SetActual("email@email.com");
	}
}

void FTPmeView::LoadSettings()
{
	BPath settings_path;
	find_directory(B_USER_SETTINGS_DIRECTORY, &settings_path);
	settings_path.SetTo(settings_path.Path(), "FTPme_settings");
	BFile *settings = new BFile();
	status_t result;
	result = settings->SetTo(settings_path.Path(), B_READ_ONLY);
	if (result == B_NO_ERROR)
	{
		const char *tempstring[100];
		int32 tempint32=0;
		BMessage *settings_msg = new BMessage(SETTINGS);
		settings_msg->Unflatten(settings);
		settings_msg->FindString("USER", tempstring);
		username->SetText(*tempstring);
		settings_msg->FindString("PASS", tempstring);
		password->SetText("*****");
		password->SetActual(*tempstring);
		settings_msg->FindString("SITE", tempstring);
		site->SetText(*tempstring);
		settings_msg->FindString("PATH", tempstring);
		path->SetText(*tempstring);
		settings_msg->FindInt32("ANON", tempint32);
		//anonymous->SetValue(tempint32);  //this doesn't work for some reason
	}
	settings->Unset();
}

void FTPmeView::SaveSettings()
{
	BPath settings_path;
	find_directory(B_USER_SETTINGS_DIRECTORY, &settings_path);
	settings_path.SetTo(settings_path.Path(), "FTPme_settings");
	/*
	settings_path.SetTo(settings_path.Path(), "FTPme");
	BDirectory FTPmeSettingsDir;
	status_t result = FTPmeSettingsDir.CreateDirectory(settings_path, FTPmeSettingsDir);
	if (result != B_OK) return;
	settings_path.SetTo(settings_path.Path(), "FTPme_settings");
	*/
	BFile *settings = new BFile(settings_path.Path(), B_READ_WRITE| B_CREATE_FILE| B_ERASE_FILE);
	
	BMessage *settings_msg = new BMessage(SETTINGS);			//make an empty BMessage
	settings_msg->AddString("USER", username->Text());				//stuff our settings into it
	settings_msg->AddString("PASS", password->actualText());
	settings_msg->AddString("SITE", site->Text());
	settings_msg->AddString("PATH", path->Text());
	settings_msg->AddInt32("ANON", anonymous->Value());
	settings_msg->Flatten(settings);												//now flatten it into a file!
}

void FTPmeView::MessageReceived(BMessage *message)
{
	switch(message->what)
	{
		case GO_EVENT:
			break;
		case ANON_EVENT:
			AnonymousToggle();
			break;
		default: ;
	}
}

void FTPmeView::AdvanceCounter() {
	activityCounter++;
	if ((activityCounter == 8) || (activityCounter == 1000)) activityCounter = 0;	
}

void FTPmeView::SetXferRate(size_t the_rate) {
	string temp = "";
	char conv;
	int t1000 = the_rate / 1000;
	int t100 = the_rate / 100;
	int t10 = the_rate / 10;
	conv = t1000;
	if (conv > 0) temp += conv + 48;
	conv = t100 - (t1000 * 10);
	if (conv > 0) temp += conv + 48;
	conv = t10 - (t100 * 10);
	if (conv > 0) temp += conv + 48;
	conv = the_rate - (t10 * 10);
	if (conv > 0) temp += conv + 48;
	temp += " KB/s";
	xferRate->SetText(temp.c_str());
}

void FTPmeView::Disable() {
	go->SetEnabled(false);
}
