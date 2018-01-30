#pragma once
#include "stdafx.h"


// Struct representing a single line for printing.
// Contains the text, indent and highlight information.
struct Line
{

public:

	#pragma region Members

	// string containing the text to print
	std::string Text;

	// Indent to the right
	int Indent = 0;

	// List containing information on the highlights.
	// First item is the starting index of the highlight.
	// Second item is the length of the highlight.
	// Third item is the colour of the highlight.
	// First highlights is dominant over next ones. It means that if the specified highlights are:
	// (0,10,5) and (6,20,8) colour 5 will be applied to indexes 0-9 and 8 will be applied to 10-25.
	std::list<std::tuple<int, int, int>> Highlights;

	#pragma endregion

	#pragma region Constructors/Destructors

	// Default constructor
	Line() { }

	// Constructor for a line without highlight. Default indent is 0
	Line(std::string text, int indent = 0);

	// Constructor for a line with a single highlight.
	Line(std::string text, int indent, int highStart, int highLength, int highColour);

	// Constructor for a line with two highlights.
	Line(std::string text, int indent, int firstHighStart, int firstHighLength, int firstHighColour,
		int secondHighStart, int secondHighLength, int secondHighColour);

	// Constructor for a line with many highlights
	Line(std::string text, int indent, std::list<std::tuple<int, int, int>> highlights);

	// Default destructor
	virtual ~Line() { }

	#pragma endregion

	#pragma region Methods

	// Prints the line to the standard output in the current position
	void Print();

	// Returns true if the line to print is empty (i.e. no text)
	bool IsEmpty() { return Text.empty(); }

	#pragma endregion

	#pragma region Operators

	// Addition operator, adds 2 Lines (adds texts, keeps indent of the first, moves the beginnings of the
	// highlights of the other by the length of the first)
	Line operator+(Line other);

	// Addition operator, adds 2 Lines (adds texts, keeps indent of the first, moves the beginnings of the
	// highlights of the other by the length of the first)
	Line operator+=(Line other);

	#pragma endregion

};