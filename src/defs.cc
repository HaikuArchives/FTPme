#include "defs.h"

void error(const char *message)
{
	BAlert *alert = new BAlert(ADDON_NAME " Error :\n", message, "OK");
	alert->Go();
}