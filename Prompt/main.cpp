/*****************************************************************************************
 * main.cpp
 *
 * Prompt main line code
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

std::vector<segmentOptions> segments;

static bool detectZsh() {
	const char* shell = std::getenv("SHELL");
	return shell != NULL && strstr(shell, "zsh") != NULL;
}

bool isZsh = detectZsh();

int main(int argc, const char* argv[]) {
	size_t			index = 0;
	renderSegment	last;
	renderSegment	rendered;
	std::string		prompt;

	getOptions();
	for (index = 0; index < segments.size(); ++index) {
		segments[index].actionFunction(segments[index], rendered, argc, argv);
		if (!rendered.text.empty()) {
			if (!prompt.empty()) {
				prompt.append(makeColor(last.backgroundColor, rendered.backgroundColor));
				if (rendered.text != " ") {
					prompt.append("\uE0B0");
				}
			}
			prompt.append(makeColor(rendered.foregroundColor, rendered.backgroundColor));
			if (rendered.text != " ") {
				prompt.append(rendered.text);
			}
			last = rendered;
			rendered.text.clear();
			rendered.backgroundColor.clear();
			rendered.foregroundColor.clear();
		}
	}

	prompt.append(makeEnd(last.backgroundColor));
	if (isZsh) {
		prompt.insert(0, "\%{\033[1m\%}");
	} else {
		prompt.insert(0, "\033[1m");
	}
	printf("%s", prompt.c_str());
	return 0;
}

std::string makeEnd(std::string& backgroundColor) {
	std::string retColor	= "\033[0m";
	std::string bcolor		= "\033[38;";
	std::string resetFColor = "\033[39m";
	std::string resetBColor = "\033[49m";

	if (isZsh) {
		retColor.insert(0, "\%{");
		retColor.append("\%}");

		resetFColor.insert(0, "\%{");
		resetFColor.append("\%}");

		resetBColor.insert(0, "\%{");
		resetBColor.append("\%} ");
	}

	if (backgroundColor.length() > 3) {
		bcolor.append("2;");
	} else {
		bcolor.append("5;");
	}
	bcolor.append(backgroundColor);
	bcolor.append("m");

	if (isZsh) {
		bcolor.insert(0, "\%{");
		bcolor.append("\%}");
	}

	retColor.append(bcolor);
	retColor.append("\uE0B0");
	retColor.append(resetFColor);
	retColor.append(resetBColor);

	return retColor;
}

std::string makeColor(std::string& foregroundColor, std::string& backgroundColor) {
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

void appendSpaceAsNeeded(std::string& str) {
	size_t len = str.length();

	if (len > 0 && !isspace(str[len - 1])) {
		str.append(" ");
	}
}
