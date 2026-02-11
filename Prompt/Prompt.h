/*****************************************************************************************
 * Prompt.h
 *
 *
 *
 * Author   :  Gary Ash <gary.ash@icloud.com>
 * Created  :   2-Feb-2026  4:30pm
 * Modified :
 *
 * Copyright © 2026 By Gary Ash All rights reserved.
 ****************************************************************************************/

#ifndef Prompt_h
#define Prompt_h

/*****************************************************************************************
 * include files
 ****************************************************************************************/
#include <map>
#include <string>
#include <string.h>
#include <vector>

/*****************************************************************************************
 * segment data structure
 ****************************************************************************************/
struct segmentOptions;
struct renderSegment;
typedef void (* segmentFunction)(segmentOptions& segment, renderSegment& rendered, int argc, const char** argv);

struct renderSegment {
	std::string text;
	std::string foregroundColor;
	std::string backgroundColor;
};

struct segmentOptions {
	std::string		foregroundColor;
	std::string		backgroundColor;
	std::string		alternateForegroundColor;
	std::string		alternateBackgroundColor;
	segmentFunction actionFunction;
};

/*****************************************************************************************
 * global function prototypes
 ****************************************************************************************/
bool getOptions();
void appendSpaceAsNeeded(std::string& str);
std::string makeEnd(std::string& backgroundColor);
std::string makeColor(std::string& foregroundColor, std::string& backgroundColor);
void segmentCwd(segmentOptions& options, renderSegment& rendered, int argc, const char** argv);
void segmentError(segmentOptions& options, renderSegment& rendered, int argc, const char** argv);
void segmentGit(segmentOptions& options, renderSegment& rendered, int argc, const char** argv);
void segmentMachine(segmentOptions& options, renderSegment& rendered, int argc, const char** argv);
void segmentUser(segmentOptions& options, renderSegment& rendered, int argc, const char** argv);
void segmentShortError(segmentOptions& options, renderSegment& rendered, int argc, const char** argv);


extern std::vector<segmentOptions> segments;

#endif /* Prompt_h */
