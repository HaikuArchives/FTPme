#include "FTPmeApp.h"
#include <Application.h>

FTPmeApp::FTPmeApp(): BApplication(FTPMEAPP_SIG)
{
	receivedRefs = false;
	launchedFromAddOn = false;
	strcpy(refsReceivedTextBuffer, "");
}

void FTPmeApp::ReadyToRun()
{
	mainWindow = new FTPmeWindow();
	mainWindow->Show();
}

void FTPmeApp::MessageReceived(BMessage *message)
{
	switch(message->what){
		default:
			BApplication::MessageReceived(message);
	}
}

void FTPmeApp::RefsReceived(BMessage *message) {
	receivedRefs = true;
	entry_ref dir_ref;

	switch(message->FindRef("dir_ref", &dir_ref)) {
		case B_OK: {
			launchedFromAddOn = true;
			#ifdef DEBUG
			error("Received; launchedFromAddOn = true");
			#endif
			
			BPath path; 
			BEntry entry(&dir_ref); 
			entry.GetPath(&path); 
			mainWindow->SetLCD(path.Path());
			break;
		}
		case B_NAME_NOT_FOUND: {
			// this case should occur when the app is launched normally, but for some
      		// unknown reason, a plain B_ERROR is generated instead.  This appears to 
			// be a bug in the BeOS.
			error("Error: B_NAME_NOT_FOUND");
			break;
		}
		case B_BAD_TYPE: {
			error("Error: B_BAD_TYPE");
			exit(B_ERROR);
			break;
		}
		case B_BAD_VALUE: {
			error("Error: B_BAD_VALUE");
			exit(B_ERROR);
			break;
		}
		default: {
			// the app was launched normally (not as a Tracker add-on)
			// see comment for B_NAME_NOT_FOUND case
			break;
		}
	}

	if(launchedFromAddOn) {
		uint32 type;
		int32 count; 
		entry_ref ref; 
 
		message->GetInfo("refs", &type, &count); 
		if(type != B_REF_TYPE) {
			return;
		}

		uint32 numberOfFiles = 0;
		uint32 numberOfDirectories = 0;
		uint32 numberOfSymLinks = 0;
        
		for (int32 i = 0; i < count && message->FindRef("refs", i, &ref) == B_OK; i++) {
			BEntry entry(&ref); 
			if (entry.IsFile()) {
				BPath filePath;
				entry.GetPath(&filePath);
				mainWindow->EnqueueFile(filePath.Path());
				numberOfFiles++;
			}
			else if (entry.IsDirectory()) {
				BPath dirPath;
				entry.GetPath(&dirPath);
				mainWindow->EnqueueDir(dirPath.Path());
				numberOfDirectories++;
			}
			else if (entry.IsSymLink()) {
				numberOfSymLinks++;
			}
		}
	
		#ifdef DEBUG
		sprintf(refsReceivedTextBuffer, "Received refs for %u file(s), %u directory(s), and %u symbolic link(s).", 
							numberOfFiles, numberOfDirectories, numberOfSymLinks);
		error(refsReceivedTextBuffer);
		#endif
	}
}