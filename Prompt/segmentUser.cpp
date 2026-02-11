/*****************************************************************************************
 * segmentUser.cpp
 *
 * The file implements a user's name segment
 *
 * Author   :  Gary Ash <gary.ash@icloud.com>
 * Created  :   2-Feb-2026  4:30pm
 * Modified :
 *
 * Copyright © 2026 By Gary Ash All rights reserved.
 ****************************************************************************************/

/*****************************************************************************************
 * include files
 ****************************************************************************************/
#include <unistd.h>
#include "Prompt.h"

void segmentUser(segmentOptions& options, renderSegment& rendered, int argc, const char** argv) {
	rendered.foregroundColor	= options.foregroundColor;
	rendered.backgroundColor	= options.backgroundColor;
	rendered.text				= getlogin();

	if (geteuid() == 0) {
		rendered.foregroundColor	= options.alternateForegroundColor;
		rendered.backgroundColor	= options.alternateBackgroundColor;
	}
}
