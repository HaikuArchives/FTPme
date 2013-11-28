#ifndef DEFS_H
#define DEFS_H

#if defined(__INTEL__)
	#define _DEBUGGER_H		// workaround for debugger.h
	#include <Be.h>
#endif

#include <Alert.h>
#include <Beep.h>
#include <stdio.h>

#define ADDON_NAME "FTPme"
#define ADDON_VERS "1.1.0"

void error(const char *message);

#endif
