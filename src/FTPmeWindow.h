#ifndef FTPMEWINDOW_H
#define FTPMEWINDOW_H

#include <Window.h>
#include <stdio.h>
#include "FTPmeView.h"
#include "defs.h"

const BRect mainWindowFrame (100,100,300,290);

class FTPmeWindow: public BWindow
{
	public:
		FTPmeWindow();
		~FTPmeWindow();
		virtual void MessageReceived(BMessage *message);
		virtual bool QuitRequested();
		void start_xfer_thread();
		static long ship_it(void *data);
		static long monitor_it(void *data);
		void EnqueueDir(const char *name);
		void EnqueueFile(const char *name);
		void ExportSendDirs(FILE *fileptr);
		void ExportSendFiles(FILE *fileptr);
		void SetLCD(const char *name);
		
		struct Line {
			char buf[B_PATH_NAME_LENGTH];
			Line *next;	
		};
	private:
		FTPmeView *mainView;
		thread_id xfer_thread;
		thread_id progress_thread;
		bool doneSending;
		int LCDlen;
		char LCD[B_PATH_NAME_LENGTH];
		Line *newline;
		Line *curr, *first, *prev;
		Line *curr_dir, *first_dir, *prev_dir;
};

#endif
