#include "stdafx.h"

// Handle to the standard output (console)
// Origin: PrintManager.cpp
extern HANDLE Console;


// Constructor for a line without highlight. Default indent is 0
Line::Line(std::string text, int indent)
{
	if (indent < 0)
	{
		throw Exception("Indent can't be smaller than 0 (passed value: " + std::to_string(indent) + ")");
	}

	Text = text;
	Indent = indent;
}

// Constructor for a line with a single highlight.
Line::Line(std::string text, int indent, int highStart, int highLength, int highColour)
	: Line(text, indent)
{
	if (highStart < 0 || highLength < 0 || highColour < 0)
	{
		throw Exception("Highlight start, length or color can't be smaller than 0 (passed values: " +
			std::to_string(highStart) + ", " + std::to_string(highLength) + ", " + std::to_string(highColour) + ")");
	}

	Highlights.push_back(std::make_tuple(highStart, highLength, highColour));
}

// Constructor for a line with two highlights.
Line::Line(std::string text, int indent, int firstHighStart, int firstHighLength, int firstHighColour,
	int secondHighStart, int secondHighLength, int secondHighColour)
	: Line(text, indent)
{
	if (firstHighStart < 0 || firstHighLength < 0 || firstHighColour < 0 ||
		secondHighStart < 0 || secondHighLength < 0 || secondHighColour < 0)
	{
		throw Exception("Highlight start, length or color can't be smaller than 0 (passed values: " +
			std::to_string(firstHighStart) + ", " + std::to_string(firstHighLength) + ", " +
			std::to_string(firstHighColour) + ", " + std::to_string(secondHighStart) + ", " +
			std::to_string(secondHighLength) + ", " + std::to_string(secondHighColour) + ")");
	}

	Highlights.push_back(std::make_tuple(firstHighStart, firstHighLength, firstHighColour));
	Highlights.push_back(std::make_tuple(secondHighStart, secondHighLength, secondHighColour));
}

// Constructor for a line with many highlights
Line::Line(std::string text, int indent, std::list<std::tuple<int, int, int>> highlights)
	: Line(text, indent)
{
	int counter = 0;

	for (auto it = highlights.begin(); it != highlights.end(); ++it, ++counter)
	{
		if (std::get<0>(*it) < 0 || std::get<1>(*it) < 0 || std::get<2>(*it) < 0)
		{
			throw Exception("Highlight start, length or color can't be smaller than 0 (passed values in " +
				std::to_string(counter) + " index element): " +
				std::to_string(std::get<0>(*it)) + ", " + std::to_string(std::get<1>(*it)) + ", " +
				std::to_string(std::get<2>(*it)) + ")");
		}
	}



	Highlights = highlights;
}

// Prints the line to the standard output in the current position
void Line::Print()
{
	// Apply the indent
	if(Indent > 0)
	{
		std::cout<<std::string(Indent, ' ');
	}

	// If there are no highlights
	if (Highlights.empty())
	{
		// Just print the text
		std::cout<<Text;
		return;
	}

	Highlights.sort(
		// Sort based on the highlight start index
		[](const std::tuple<int, int, int>& first, const std::tuple<int, int, int>& second)
		{ return std::get<0>(first) < std::get<0>(second); });

	int currentIndex = 0;

	for (auto it = Highlights.begin(); it != Highlights.end(); ++it)
	{
		// If there's text preceding the current highlight
		if (currentIndex < std::get<0>(*it))
		{
			// Print it and go the the highlight start index
			std::cout<<Text.substr(currentIndex, std::get<0>(*it) - currentIndex);
			currentIndex = std::get<0>(*it);
		}

		// Get the substring to print
		std::string substrToPrint = Text.substr(currentIndex, std::get<1>(*it));

		// Set the highlight
		SetConsoleTextAttribute(Console, std::get<2>(*it));

		// Print the substring
		std::cout<<substrToPrint;

		// Roll back the text
		SetConsoleTextAttribute(Console, NORMAL_COLOR);

		// Move the currentIndex
		currentIndex += substrToPrint.length();

		// If the currentIndex reached the end (or for some reason exceede it) break the loop early as there's
		// no more text to print
		if (currentIndex >= Text.length())
		{
			break;
		}
	}

	// If it's not the end, print the rest of the string
	if (currentIndex != Text.length())
	{
		std::cout<<Text.substr(currentIndex, std::string::npos);
	}
}


// Addition operator, adds 2 Lines (adds texts, keeps indent of the first, moves the beginnings of the
// highlights of the other by the length of the first)
Line Line::operator+(Line other) 
{
	// Get the first's highlights
	std::list<std::tuple<int, int, int>> highlights = Highlights;

	// And the others
	std::list<std::tuple<int, int, int>> otherHighlights = other.Highlights;

	int length = Text.length();

	// Move each highlight in the other by the length of the text from the first
	for (auto it = otherHighlights.begin(); it != otherHighlights.end(); ++it)
	{
		std::get<0>(*it) += length;
	}

	highlights.splice(highlights.end(), otherHighlights);

	return Line(Text+other.Text, Indent, highlights);
}


// Addition operator, adds 2 Lines (adds texts, keeps indent of the first, moves the beginnings of the
// highlights of the other by the length of the first)
Line Line::operator+=(Line other)
{
	// Get the other's highlights
	std::list<std::tuple<int, int, int>> otherHighlights = other.Highlights;

	int length = Text.length();

	// Move each highlight in the other by the length of the text from the first
	for (auto it = otherHighlights.begin(); it != otherHighlights.end(); ++it)
	{
		std::get<0>(*it) += length;
	}

	Highlights.splice(Highlights.end(), otherHighlights);

	Text += other.Text;

	return *this;	
}
