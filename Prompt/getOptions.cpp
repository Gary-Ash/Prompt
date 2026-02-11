/*****************************************************************************************
 * getOptions.cpp
 *
 * This file contains the code that parses the options in SNAZZY_PROMPT environment
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

static std::map<std::string, segmentFunction> supportedSegments = {
	{ "cwd",	 segmentCwd		   },
	{ "err",	 segmentError	   },
	{ "git",	 segmentGit		   },
	{ "machine", segmentMachine	   },
	{ "short",	 segmentShortError },
	{ "user",	 segmentUser	   },
};

static void getEnvironmentVariables(char *options) {
	bool		trueColor	= false;
	const char* colorTerm	= std::getenv("COLORTERM");

	if (colorTerm != NULL) {
		if (strcmp(colorTerm, "truecolor") == 0 || strcmp(colorTerm, "24bit") == 0) {
			trueColor = true;
		}
	}

	if (trueColor == true) {
		const char* env = std::getenv("SNAZZY_PROMPT_TRUE");
		if (env == NULL) {
			strcpy(options, "user,255;255;255,106;168;214;255;255;255,106;168;214:machine,255;255;255,180;167;214:cwd,255;255;255,255;148;0,255;255;255,1:git,255;255;255,147;196;124,255;255;255,255;142;198:short,255;255;255,128;0;0");
		} else {
			strcpy(options, env);
		}
	} else {
		const char* env = std::getenv("SNAZZY_PROMPT");
		if (env == NULL) {
			strcpy(options, "user,255,39,255,1:machine,255,219:cwd,255,45,255,166:git,255,35,255,200:short,255,166");
		} else {
			strcpy(options, env);
		}
	}
}

bool getOptions() {
	char			options[ARG_MAX];
	segmentOptions	theSegmentOptions;

	char*			parsePoint		= NULL;
	char*			segmentChunk	= NULL;
	char*			colorChunk		= NULL;
	char*			commaPtr		= NULL;
	char*			semicolonPtr	= NULL;

	getEnvironmentVariables(options);
	parsePoint = options;
	while ((parsePoint = strtok_r(parsePoint, ":", &segmentChunk)) != NULL) {
		// get the segment name
		if ((commaPtr = strchr(parsePoint, ',')) == NULL) {
			fprintf(stderr, "Error options variable unable to find segment name - starting here - %s\n", parsePoint);
			return false;
		}
		std::string segmentName = std::string(parsePoint, (commaPtr - parsePoint));
		auto		it			= supportedSegments.find(segmentName);
		if (it != supportedSegments.end()) {
			colorChunk							= ++commaPtr;
			theSegmentOptions.actionFunction	= it->second;

			// parse the color specs
			while ((colorChunk = strtok_r(colorChunk, ",", &commaPtr)) != NULL) {
				std::string colorSpecification;

				if ((semicolonPtr = strchr(colorChunk, ';')) == NULL) {
					int color = atoi(colorChunk);
					if (color < 0 || color > 255) {
						fprintf(stderr, "Error bad true color specification in option - starting here - %s\n", parsePoint);
						return false;
					}
				} else {
					int r, g, b;
					if (sscanf(colorChunk, "%d;%d;%d", &r, &g, &b) != 3 ||
						((r < 0 || r > 255) || (g < 0 || g > 255) || (b < 0 || b > 255))) {
						fprintf(stderr, "Error bad true color specification (range error) in option - starting here - %s\n", parsePoint);
						return false;
					}
				}
				colorSpecification = colorChunk;

				if (theSegmentOptions.foregroundColor.empty()) {
					theSegmentOptions.foregroundColor = colorSpecification;
				} else if (theSegmentOptions.backgroundColor.empty()) {
					theSegmentOptions.backgroundColor = colorSpecification;
				} else if (theSegmentOptions.alternateForegroundColor.empty()) {
					theSegmentOptions.alternateForegroundColor = colorSpecification;
				} else if (theSegmentOptions.alternateBackgroundColor.empty()) {
					theSegmentOptions.alternateBackgroundColor = colorSpecification;
				}
				colorChunk = commaPtr;
			}
		} else {
			fprintf(stderr, "Error options variable unknown segment name - starting here - %s\n", parsePoint);
			return false;
		}
		segments.push_back(theSegmentOptions);
		theSegmentOptions.foregroundColor.clear();
		theSegmentOptions.backgroundColor.clear();
		theSegmentOptions.alternateForegroundColor.clear();
		theSegmentOptions.alternateBackgroundColor.clear();
		theSegmentOptions.actionFunction = NULL;

		parsePoint = segmentChunk;
	}
	return true;
}
