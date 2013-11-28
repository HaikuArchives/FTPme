#ifndef FTPMEVIEW_H
#define FTPMEVIEW_H

#include "defs.h"
#include <View.h>
#include <Point.h>
#include <TextControl.h>
#include <Button.h>
#include <CheckBox.h>
#include "PassControl.h"
#include <StringView.h> //new

const pattern stripes8 = {{0x1e, 0x3c, 0x78, 0xf0, 0xe1, 0xc3, 0x87, 0x0f}};
const pattern stripes7 = {{0x3c, 0x78, 0xf0, 0xe1, 0xc3, 0x87, 0x0f, 0x1e}};
const pattern stripes6 = {{0x78, 0xf0, 0xe1, 0xc3, 0x87, 0x0f, 0x1e, 0x3c}};
const pattern stripes5 = {{0xf0, 0xe1, 0xc3, 0x87, 0x0f, 0x1e, 0x3c, 0x78}};
const pattern stripes4 = {{0xe1, 0xc3, 0x87, 0x0f, 0x1e, 0x3c, 0x78, 0xf0}};
const pattern stripes3 = {{0xc3, 0x87, 0x0f, 0x1e, 0x3c, 0x78, 0xf0, 0xe1}};
const pattern stripes2 = {{0x87, 0x0f, 0x1e, 0x3c, 0x78, 0xf0, 0xe1, 0xc3}};
const pattern stripes1 = {{0x0f, 0x1e, 0x3c, 0x78, 0xf0, 0xe1, 0xc3, 0x87}};


#define GO_EVENT 'GO'
#define ANON_EVENT 'ANON'
#define SETTINGS 'SETT'
const BRect mainViewFrame (0,0,200,190);
const BRect usernameFrame (5,5,195,20);
const BRect passwordFrame (5,30,195,45);
const BRect siteFrame (5,55,195,70);
const BRect pathFrame (5,80,195,95);
const BRect anonymousFrame (5,105,195,120);
const BRect logFrame (5,130,195,145);
const BRect goButtonFrame (150,153,190,168);

//const BRect statusFrame(5,155,140,170);
const BRect statusFrame(150,105,190,120);

const BRect progressFrame(7,158,135,172);
const BRect shadowFrame(8,159,136,173);
const BRect shadowFrame2(9,160,137,174);

const BPoint bar_topleft(8,159);
const BPoint bar_topright(134,159);
const BPoint bar_bottomleft(8,171);
const BPoint bar_bottomright(134,171);


class FTPmeView: public BView
{
	public:
		FTPmeView();
		virtual void MessageReceived(BMessage *message);
		virtual void Draw(BRect updateRect); //new!
		void AdvanceCounter();
		void SetXferRate(size_t the_rate);
		void Disable();
		void AnonymousToggle();
		void LoadSettings();
		void SaveSettings();
		const char *Username() const;
		const char *ActualPassword() const;
		const char *Site() const;
		const char *Path() const;
		int32 LogValue() const;
		BButton *go;
		
	private:
		BTextControl *username;
		PassControl *password;
		BTextControl *site;
		BTextControl *path;
		
		BCheckBox *anonymous;
		BCheckBox *log;
		
		char tempuser[100];
		char temppass[100];
		size_t activityCounter;
		BStringView *xferRate;
};

#endif
