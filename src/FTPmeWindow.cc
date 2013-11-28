// FILE: FTPmeWindow.cc
// Implements the FTPmeWindow class

#include "FTPmeWindow.h"
#include <String.h>
#include <fstream.h>
#include <time.h>
#include <Point.h>
#include <string>
#include <Font.h>

FTPmeWindow::FTPmeWindow():
BWindow(mainWindowFrame, "FTPme: Hello", B_TITLED_WINDOW, B_NOT_RESIZABLE|B_NOT_ZOOMABLE)
{
	doneSending = false;
	Lock();
	first = NULL;
	curr = NULL;
	
	mainView = new FTPmeView();
	AddChild(mainView);
	SetDefaultButton(mainView->go);
	Unlock();
}

FTPmeWindow::~FTPmeWindow()
{
	curr = first;
	while (curr != NULL)
	{
		prev = curr;
		curr = curr->next;
		delete prev;
	}
	
	curr_dir = first_dir;
	while (curr_dir != NULL)
	{
		prev_dir = curr_dir;
		curr_dir = curr_dir->next;
		delete prev_dir;
	}
}

void FTPmeWindow::MessageReceived(BMessage *message)
{
		switch(message->what)
		{
		case GO_EVENT:
			start_xfer_thread();
			break;
		case ANON_EVENT:
			BWindow::DispatchMessage(message, mainView);
			break;
		default:
			BWindow::MessageReceived(message);
		}
}

bool FTPmeWindow::QuitRequested()
{
	be_app->PostMessage(B_QUIT_REQUESTED);
	kill_thread(xfer_thread);
	kill_thread(progress_thread);
	return BWindow::QuitRequested();
}

void FTPmeWindow::EnqueueFile(const char *name)
{
	BString tempstring = name;
	tempstring.Remove(0,LCDlen + 1);
	
	newline = new Line;
	sprintf(newline->buf, "send \"%s\"\n", tempstring.String());
	newline->next = NULL;
	
	if (first == NULL) {
		first = newline;
		curr = newline; }
	else {
		curr->next = newline;
		curr = newline;
	}
}

void FTPmeWindow::EnqueueDir(const char *name)
{
	BString tempstring = name;
	tempstring.Remove(0,LCDlen + 1);
	
	newline = new Line;
	sprintf(newline->buf, "mkdir \"%s\"\n", tempstring.String());
	newline->next = NULL;
	
	if (first_dir == NULL) {
		first_dir = newline;
		curr_dir = newline; }
	else {
		curr_dir->next = newline;
		curr_dir = newline;
	}
	
	BEntry temp;
	BDirectory base_dir(name);
	
	while (base_dir.GetNextEntry(&temp) != B_ENTRY_NOT_FOUND) {
		if (temp.IsDirectory()) {
			BPath dirPath;
			temp.GetPath(&dirPath);
			EnqueueDir(dirPath.Path());
		}
		else if (temp.IsFile()) {
			BPath filePath;
			temp.GetPath(&filePath);
			EnqueueFile(filePath.Path());
		}
		else {
		}
	}
}

void FTPmeWindow::SetLCD(const char *name)
{
	sprintf(LCD, "%s", name);
	LCDlen = strlen(LCD);
}

void FTPmeWindow::ExportSendDirs(FILE *fileptr)
{
	curr_dir = first_dir;
	while (curr_dir != NULL) {
		fprintf(fileptr, curr_dir->buf);
		curr_dir = curr_dir->next;
	}
}

void FTPmeWindow::ExportSendFiles(FILE *fileptr)
{
	curr = first;
	while (curr != NULL) {
		fprintf(fileptr, curr->buf);
		curr = curr->next;
	}
}

void FTPmeWindow::start_xfer_thread()
{
	xfer_thread = spawn_thread(FTPmeWindow::ship_it, "Transfer Process",
					B_NORMAL_PRIORITY, (void*)this);
	resume_thread(xfer_thread);
	progress_thread = spawn_thread(FTPmeWindow::monitor_it, "Progress Monitor",
					B_URGENT_DISPLAY_PRIORITY, (void*)this);
	resume_thread(progress_thread);
}

long FTPmeWindow::ship_it(void *data)
{
	FTPmeWindow *w;
	w = (FTPmeWindow*)data;
	 w->Lock();
	 w->mainView->Disable();  //to prevent starting multiple uploads
	 w->Unlock();
	//check for filename conflict
	FILE *fp;
	if ((fp = fopen("FTPme.inp", "r")) != NULL) {
		fclose(fp);
		system("rm FTPme.inp");
		//error("Error opening file: FTPme.inp already exists");
		//return(1);
	}
	//if no conflict, create the file
	w->SetTitle("FTPme: Queueing...");
	if ((fp = fopen("FTPme.inp", "w")) != NULL) {
		w->Lock();
		fprintf(fp, "open %s\n", w->mainView->Site());
		fprintf(fp, "%s\n", w->mainView->Username());
		fprintf(fp, "%s\n", w->mainView->ActualPassword());
		fprintf(fp, "cd \"%s\"\n", w->mainView->Path());
		fprintf(fp, "lcd \"%s\"\n", w->LCD);
		w->Unlock();
		
		//test of progress monitoring
		fprintf(fp, "hash\n");
		
		//'send' commands here
		w->ExportSendDirs(fp);
		w->ExportSendFiles(fp);
		
		fprintf(fp, "bye\n");
		fclose(fp);
		w->SetTitle("FTPme: Sending...");
			system("cat FTPme.inp | ftp > FTPme.log");
		w->SetTitle("FTPme: Cleaning Up...");
		#ifdef DEBUG
			error("about to remove input file");
		#endif
		remove("FTPme.inp");
		w->Lock();
		if (w->mainView->LogValue() == B_CONTROL_OFF) {
			w->doneSending = true;
			kill_thread(w->progress_thread);
			remove("FTPme.log");
		}
		//w->Lock();
		w->mainView->SaveSettings();	
		w->Unlock();
		w->QuitRequested();
	}
	return(0);
}


void Seek(ifstream &thestream, size_t howfar) {
	char ch;
	size_t currentindex = 0;
	while ((!thestream.eof()) && (currentindex < howfar)) {
		thestream.get(ch);
		currentindex++;
	}
}

long FTPmeWindow::monitor_it(void *data)
{
	FTPmeWindow *w;
	w = (FTPmeWindow*)data;
	
	status_t errorcode;
	BFile logfile;
	
	errorcode = logfile.SetTo("/boot/home/FTPme.log", B_READ_ONLY);
	while (errorcode != B_NO_ERROR) {
		//keep trying to open the file until it succeeds
		snooze(500000);
		errorcode = logfile.SetTo("/boot/home/FTPme.log", B_READ_ONLY);
	}
	
	off_t theSize;
	size_t theNewSize = 0;
	size_t theOldSize = 0;
	size_t timer = 0;
	size_t keepSize = 0;
	size_t theRate = 0;
	
	BRect clearFrame(7,158,135,172);
	w->Lock();
	while (!(w->doneSending)) {
		w->Unlock();
		
		logfile.GetSize(&theSize);
		theNewSize = theSize;
		
		// RATE STUFF
		if (timer == 0) keepSize = theSize;
		timer++;
		if (timer == 20) {  //has it been a second?
				timer = 0;
				theRate = theSize - keepSize;
				w->Lock();
				w->mainView->SetXferRate(theRate);
				w->Unlock();
		}
		// /RATE STUFF
		
		if (theNewSize != theOldSize) {
			w->Lock();
			
			w->mainView->AdvanceCounter();		
			w->mainView->Invalidate(clearFrame);
			//w->UpdateIfNeeded();
						
			w->Unlock();
		}
		
		snooze(50000);
				
		theOldSize = theNewSize;
		w->Lock();
	}
	return(0);
}



