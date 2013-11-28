#ifndef FTPMEAPP_H
#define FTPMEAPP_H

#include <Application.h>
#include "FTPmeWindow.h"

#define FTPMEAPP_SIG "application/x-vnd.Tysoft-FTPme"

class FTPmeApp: public BApplication
{
	public:
		FTPmeApp();
		virtual void ReadyToRun();
		virtual void MessageReceived(BMessage *message);
		virtual void RefsReceived(BMessage *message);
		bool receivedRefs;
		bool launchedFromAddOn;
		char refsReceivedTextBuffer[1024];
		
	private:
		FTPmeWindow *mainWindow;

};

#endif