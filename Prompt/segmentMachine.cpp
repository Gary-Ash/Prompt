/*****************************************************************************************
 * segmentMachine.cpp
 *
 * The file implements a user's machine/host name segment
 *
 * Author   :  Gary Ash <gary.ash@icloud.com>
 * Created  :  20-Mar-2025 10:54pm
 * Modified :
 *
 * Copyright © 2024 By Gary Ash All rights reserved.
 ****************************************************************************************/

/*****************************************************************************************
 * include files
 ****************************************************************************************/
#include <unistd.h>
#include "Prompt.h"

void segmentMachine(segmentOptions& options, renderSegment& rendered, int argc, const char** argv)
{
	char host[_POSIX_HOST_NAME_MAX];

	rendered.foregroundColor	= options.foregroundColor;
	rendered.backgroundColor	= options.backgroundColor;

	gethostname(host, _POSIX_HOST_NAME_MAX);
	char* p = strrchr(host, '.');
	if (p) {
		*p = '\0';
	}
	rendered.text = host;
}
