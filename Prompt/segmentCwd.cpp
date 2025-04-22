/*****************************************************************************************
 * segmentCwd.cpp
 *
 *
 *
 * Author   :  Gary Ash <gary.ash@icloud.com>
 * Created  :  20-Mar-2025 10:54pm
 * Modified :
 *
 * Copyright © 2025 By Gary Ash All rights reserved.
 ****************************************************************************************/

/*****************************************************************************************
 * include files
 ****************************************************************************************/
#include <pwd.h>
#include <unistd.h>

#include "Prompt.h"

void segmentCwd(segmentOptions& options, renderSegment& rendered, int argc, const char** argv) {
	struct knownDir { std::string symbol; std::string directory; };

	char			dir[PATH_MAX];
	struct passwd*	pw		= getpwuid(getuid());
	std::string		homeDir = pw->pw_dir;

	struct knownDir knownDirectories[] = {
		{ std::string("􀣰"), std::string(homeDir + "/Desktop")	},
		{ std::string("􀈷"), std::string(homeDir + "/Documents") },
		{ std::string("􀤨"), std::string(homeDir + "/Library")	},
		{ std::string("􀙄"), std::string(homeDir + "/Developer") },
		{ std::string("􀁸"), std::string(homeDir + "/Downloads") },
		{ std::string(""), std::string(homeDir + "/Music")		},
		{ std::string("􀎶"), std::string(homeDir + "/Movies")	},
		{ std::string(""), std::string(homeDir + "/Pictures")	},
		{ std::string(""), std::string(homeDir + "/Sites")		},

		// bottom of the list on purpoae
		{ std::string("􂘞"), std::string(homeDir)				},
	};


	memset(dir, 0, PATH_MAX);
	getcwd(dir, PATH_MAX);
	char* p = dir;

	if (access(dir, W_OK)) {
		rendered.text				= "";
		rendered.foregroundColor	= options.alternateForegroundColor;
		rendered.backgroundColor	= options.alternateBackgroundColor;
	} else {
		rendered.foregroundColor	= options.foregroundColor;
		rendered.backgroundColor	= options.backgroundColor;
	}
	for (int i = 0; i < sizeof(knownDirectories) / sizeof(knownDir); i++) {
		if (strncmp(knownDirectories[i].directory.c_str(), dir, knownDirectories[i].directory.length()) == 0) {
			rendered.text.append(knownDirectories[i].symbol);
			p = dir + knownDirectories[i].directory.length() + 1;
			if (*p != '\0') {
				rendered.text.append(" ");
			}
			break;
		}
	}

	if (strlen(p) > 30) {
		p = strrchr(p, '/');
		rendered.text.append(" ");
		if (strlen(p) > 30) {
			rendered.text.append("/…");
			p = (p + strlen(p)) - 30;
		}
	}
	rendered.text.append(" ");
	rendered.text.append(p);

	if (*p == '\0') {
		rendered.text.append(" ");
	}
}

