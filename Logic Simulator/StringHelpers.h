#pragma once
#include "stdafx.h"


// Replaces all occurances of toReplace with replaceWith. It will also replace all occurances that were
// created due to previous replacements. If toReplace is a substring of replaceWith, exception will be thrown
std::string ReplaceSubstr(std::string original, std::string toReplace, std::string replaceWith);

// Trims the string (removes spaces on the beginning and end)
std::string Trim(std::string original);

// Splits the string for substrings based on the separation by c. Doesn't include c in any of the substrings
std::vector<std::string> Split(std::string str, char c);
