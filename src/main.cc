// FILE: main.cc
// Instantiates an App object and Runs it

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <TrackerAddOn.h>
#include "FTPmeApp.h"

int main(int, char **)
{
	FTPmeApp theApp;
	theApp.Run();
	
	return(0);
}

//********************************************************
void process_refs(entry_ref dir_ref, BMessage* msg, void*)
//********************************************************
/*
	this is the function the Tracker call when the add-on is selected
	dir_ref is the folder you have launched it from
	msg is a BMessage containing the references to BEntry's(Files/Folders)
	and for "void *", I don't know yet...
*/
{
	
	uint32 type;
	int32 count; 
	entry_ref ref; 
	
	msg->GetInfo("refs", &type, &count); 
	if(type != B_REF_TYPE) {
		return;
	}
	
	if(msg->AddRef("dir_ref", &dir_ref) != B_OK) {
		error("Could not add dir_ref to message.");
		return;
	}
	//if(msg->what == B_REFS_RECEIVED) error ("intact");
	switch(be_roster->Launch(FTPMEAPP_SIG, msg)) { // launch this program as an application and send it the refs message
		case B_OK: { // launched okay - do nothing
			BMessenger *link = new BMessenger(FTPMEAPP_SIG);
			link->SendMessage(msg);
			//error("Sent the Message");
			//Post message in here maybe?
			break;
		}
		case B_BAD_VALUE: {
			error("Could not launch the application corresponding to the Tracker add-on.");
			break;
		}
		case B_ALREADY_RUNNING: { // already running - do nothing
			error("App already running.");
			break;
		}
		case B_LAUNCH_FAILED: {
			error("Could not launch the application corresponding to the Tracker add-on.");
			break;
		}
		default: { // a file system error
			error("Could not launch the application corresponding to the Tracker add-on.");
			break;
		}
	}
}


