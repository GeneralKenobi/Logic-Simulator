#include "stdafx.h"


// Replaces all occurances of toReplace with replaceWith. It will also replace all occurances that were
// created due to previous replacements. If toReplace is a substring of replaceWith, exception will be thrown
std::string ReplaceSubstr(std::string original, std::string toReplace, std::string replaceWith)
{
	if (replaceWith.find(toReplace) != std::string::npos)
	{
		throw Exception("toReplace can't be a substring of replaceWith");
	}

	int position;

	while ((position = original.find(toReplace)) != std::string::npos)
	{
		original = original.substr(0, position) + replaceWith + original.substr(position + toReplace.length());
	}

	return original;
}


// Trims the string (removes spaces on the beginning and end)
std::string Trim(std::string original)
{
	if (original.find_first_not_of(' ') == std::string::npos)
	{
		// If the string is empty or there are only spaces, return an empty string
		return "";
	}

	original = original.substr(original.find_first_not_of(' '));

	original = original.substr(0,original.find_last_not_of(' ') + 1);

	return original;
}


// Splits the string for substrings based on the separation by c. Doesn't include c in any of the substrings
std::vector<std::string> Split(std::string str, char c)
{
	std::vector<std::string> split;
	int pos = -1;

	// Where there still are occurances of c
	while ((pos = str.find(c)) != std::string::npos)
	{
		// Get the first substring
		std::string substr = str.substr(0, pos);

		// If it isn't empty
		if(!substr.empty())
		{
			// Add it to the result
			split.push_back(str.substr(0, pos));
		}

		// And erase it with c from the string
		str.erase(0,pos+1);
	}

	// If there was something left in the string
	if(!str.empty())
	{
		// Add it to the result
		split.push_back(str);
	}

	return split;
}