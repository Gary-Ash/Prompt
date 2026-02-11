/*****************************************************************************************
 * segmentCwd.cpp
 *
 *
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
#include <pwd.h>
#include <unistd.h>

#include "Prompt.h"

void segmentCwd(segmentOptions& options, renderSegment& rendered, int argc, const char** argv) {
	struct knownDir { std::string symbol; std::string directory; };

	char			dir[PATH_MAX];
	struct passwd*	pw		= getpwuid(getuid());
	std::string		homeDir = pw->pw_dir;

	struct knownDir knownDirectories[] = {
		{ std::string("\U001008F0"), std::string(homeDir + "/Desktop")	},
		{ std::string("\U00100237"), std::string(homeDir + "/Documents") },
		{ std::string("\U00100928"), std::string(homeDir + "/Library")	},
		{ std::string("\U00100644"), std::string(homeDir + "/Developer") },
		{ std::string("\U00100078"), std::string(homeDir + "/Downloads") },
		{ std::string("\uF001"), std::string(homeDir + "/Music")		},
		{ std::string("\U001003B6"), std::string(homeDir + "/Movies")	},
		{ std::string("\uF03E"), std::string(homeDir + "/Pictures")	},
		{ std::string("\uF267"), std::string(homeDir + "/Sites")		},

		// bottom of the list on purpoae
		{ std::string("\U0010261E"), std::string(homeDir)				},
	};


	memset(dir, 0, PATH_MAX);
	getcwd(dir, PATH_MAX);
	char* p = dir;

	if (access(dir, W_OK)) {
		rendered.text				= "\uF023";
		rendered.foregroundColor	= options.alternateForegroundColor;
		rendered.backgroundColor	= options.alternateBackgroundColor;
	} else {
		rendered.foregroundColor	= options.foregroundColor;
		rendered.backgroundColor	= options.backgroundColor;
	}
	for (int i = 0; i < sizeof(knownDirectories) / sizeof(knownDir); i++) {
		if (strncmp(knownDirectories[i].directory.c_str(), dir, knownDirectories[i].directory.length()) == 0) {
			rendered.text.append(knownDirectories[i].symbol);
			size_t dirLen = knownDirectories[i].directory.length();
			if (dir[dirLen] == '/') {
				p = dir + dirLen + 1;
				rendered.text.append(" ");
			} else {
				p = dir + dirLen;
			}
			break;
		}
	}

	if (strlen(p) > 30) {
		char* slash = strrchr(p, '/');
		if (slash != NULL) {
			p = slash;
		}
		rendered.text.append(" ");
		if (strlen(p) > 30) {
			rendered.text.append("/\u2026");
			p = (p + strlen(p)) - 30;
		}
	}
	rendered.text.append(" ");
	rendered.text.append(p);

	if (*p == '\0') {
		rendered.text.append(" ");
	}
}

