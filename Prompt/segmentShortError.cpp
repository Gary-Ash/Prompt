/*****************************************************************************************
 * segmentShortError.cpp
 *
 * The file implements a segment the displays the error code status of the last command
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
#include "Prompt.h"

void segmentShortError(segmentOptions& options, renderSegment& rendered, int argc, const char** argv) {
	for (int index = 0; index < argc; ++index) {
		if (strcmp(argv[index], "--error") == 0) {
			++index;
			if (index < argc) {
				int error = atoi(argv[index]);
				if (error == 0) {
					return;
				}
				rendered.text				= " ";
				rendered.foregroundColor	= options.foregroundColor;
				rendered.backgroundColor	= options.backgroundColor;
				break;
			}
		}
	}
}

