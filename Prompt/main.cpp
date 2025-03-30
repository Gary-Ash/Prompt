/*****************************************************************************************
 * main.cpp
 *
 * Prompt main line code
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
#include "Prompt.h"

std::vector<segmentOptions> segments;
bool						isZsh = (strstr(std::getenv("SHELL"), "zsh") != NULL) ? true : false;

int main(int argc, const char* argv[])
{
	size_t			index = 0;
	renderSegment	last;
	renderSegment	rendered;
	std::string		prompt;

	getOptions();
	for (index = 0; index < segments.size(); ++index) {
		last = rendered;
		rendered.text.clear();
		segments[index].actionFunction(segments[index], rendered, argc, argv);
		if (!rendered.text.empty()) {
			if (!prompt.empty()) {
				prompt.append(makeColor(last.backgroundColor, rendered.backgroundColor));
				prompt.append("");
			}
			prompt.append(makeColor(rendered.foregroundColor, rendered.backgroundColor));
			prompt.append(rendered.text);
		}
	}

	prompt.append(makeEnd(rendered.backgroundColor));
	if (isZsh) {
		prompt.append("\%{\033[0;m \%}");
		prompt.insert(0, "\%{\033[1;m\%}");
	} else {
		prompt.append("\033[0;m ");
		prompt.insert(0, "\033[1;m");
	}
	printf("%s", prompt.c_str());
	return 0;
}

std::string makeEnd(std::string& backgroundColor)
{
	std::string retColor = "\033";

	retColor.append("[38;");
	if (backgroundColor.length() > 3) {
		retColor.append("2;");
	} else {
		retColor.append("5;");
	}
	retColor.append(backgroundColor);
	retColor.append(";49m");

	if (isZsh) {
		retColor.append("\%}");
		retColor.insert(0, "\%{");
	}
	return retColor;
}

std::string makeColor(std::string& foregroundColor, std::string& backgroundColor)
{
	std::string retColor = "\033";

	retColor.append("[38;");
	if (foregroundColor.length() > 3) {
		retColor.append("2;");
	} else {
		retColor.append("5;");
	}
	retColor.append(foregroundColor);

	retColor.append(";48;");
	if (backgroundColor.length() > 3) {
		retColor.append("2;");
	} else {
		retColor.append("5;");
	}
	retColor.append(backgroundColor);
	retColor.append("m");

	if (isZsh) {
		retColor.append("\%}");
		retColor.insert(0, "\%{");
	}
	return retColor;
}

void appendSpaceAsNeeded(std::string& str)
{
	size_t len = str.length();

	if (len > 0 && !isspace(str[len - 1])) {
		str.append(" ");
	}
}
