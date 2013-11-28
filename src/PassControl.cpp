#include "PassControl.h"
#include "myFilter.h"
#ifndef _FONT_H
#include <Font.h>
#endif

#include <stdio.h>
#include <string.h>

PassControl::PassControl(BRect frame,
						const char *name,
						const char *label, 
						const char *initial_text, 
						BMessage *message)
			: BTextControl(frame, name, label, initial_text, message)

{
	length = 0;
	filter = new myFilter();
	((BTextView *)TextView())->AddFilter(filter);
	
	// font-sensitive code
	// needs for the Width() to be sufficient. this code just makes room 
	// for the label...
//	SetDivider(StringWidth(label) + 2.5);
	// ugh. forgive me for magic number usage here
	actual = new char [actual_size = MAGIC_SIZE]; // YOU define MAGIC_SIZE in PassControl.h
	memset(actual, 0, MAGIC_SIZE);
}

PassControl::~PassControl()
{
	if(((BTextView *)TextView())->RemoveFilter(filter))
	{
		delete filter;
	}
	// else leak();
	delete [] actual;
}

void PassControl::PopChar()
{
	while (length > 0)
	{
		uchar c = actual[--length];
		actual[length] = 0;
		
		// Handle UTF8:
		// If it's not an extended character, quit
		if (!(c & 0x80))
			break;

		// It is extended, so don't stop until we find the first one in the sequence.
		if (c & 0x40)
			break;
	}
}

void PassControl::PushChar(BMessage *msg)
{
	type_code type;
	int32 count;
	msg->GetInfo("byte", &type, &count);
	
	// "Grow" the buffer if necessary.  Just double it.  Won't happen often.
	if (length + count + 1 > actual_size)
	{
		char* newbuf = new char[actual_size << 1];
		memcpy(newbuf, actual, actual_size);
		delete [] actual;
		actual = newbuf;
		actual_size <<= 1;
	}	

	// Stick in all the bytes of this character.
	for (int32 x = 0; x < count; x++)
	{
		int8 byte;
		msg->FindInt8("byte", x, &byte);
		actual[length++] = byte;
		actual[length]=0;					//	Added by MOAH 06 Sept 1998
	}
}

const char *PassControl::actualText() const
{ return actual; }


void PassControl::SetActual(const char *buf)
{
	sprintf(actual, buf);
	return;

}
