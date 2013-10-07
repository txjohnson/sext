//
//  validate.cpp
//  validateschema
//
//  Created by Theo Johnson on 10/2/13.
//  Copyright (c) 2013 Theo Johnson. All rights reserved.
//

#include "validate.h"
using namespace std;

// match text should be native to the library
bool SchemaValidator:: matchText (const se::Node& t) {
	if (t.isList()) return false;

	const se::range& r = t.getBytes();
	return (r.beg != r.end);
};

// match text should be native to the library
bool SchemaValidator:: matchNumber (const se::Node& t) {
	if (t.isList()) return false;

	const se::range& r = t.getBytes();
	return (r.beg != r.end);
};

// match list text should be native to the library
bool SchemaValidator:: matchListText (const se::Node& t) {
	if (!t.isList()) return false;
	for (auto i : t.children()) {
		if (! matchText (*i))                 { log << "Line " << t.getLine() << ", expecting a list here.\n"; return false; }
	}
	return true;
}

bool SchemaValidator:: matchMany (const se::Node& t) {
	if (t != "many")                         { return false; }
	if (t.children().size() != 1) 			 { log << "Line " << t.getLine() << ", many takes 1 argument.\n"; return false; }
	
	auto list = t.children();
	if (! matchElementExpr (*list[0]))       { log << "Line " << t.getLine() << ", expecting an element spec here.\n"; return false; }
	return true;
}

bool SchemaValidator:: matchSome (const se::Node& t) {
	if (t != "some")                         { return false; }
	if (t.children().size() != 1) 			 { log << "Line " << t.getLine() << ", some takes 1 argument.\n"; return false; }
	
	auto list = t.children();
	if (! matchElementExpr (*list[0]))       { log << "Line " << t.getLine() << ", expecting an element spec here.\n"; return false; }
	return true;
}

bool SchemaValidator:: matchMaybe (const se::Node& t) {
	if (t != "maybe")                        { return false; }
	if (t.children().size() != 1) 			 { log << "Line " << t.getLine() << ", maybe takes 1 argument.\n"; return false; }
	
	auto list = t.children();
	if (! matchElementExpr (*list[0]))       { log << "Line " << t.getLine() << ", expecting an element spec here.\n"; return false; }
	return true;
}

bool SchemaValidator:: matchLtQuantifier (const se::Node& t) {
	if (t != "<")                            { return false; }
	if (t.children().size() != 2) 			 { log << "Line " << t.getLine() << ", '<' takes 2 arguments.\n"; return false; }
	
	auto list = t.children();
	if (! matchNumber (*list[0]))            { log << "Line " << list[1]->getLine() << ", expecting a number here.\n"; return false; }
	if (! matchElementExpr (*list[1]))       { log << "Line " << list[2]->getLine() << ", expecting an element spec here.\n"; return false; }
	return true;
}
bool SchemaValidator:: matchGtQuantifier (const se::Node& t) {
	if (t != ">")                            { return false; }
	if (t.children().size() != 2) 			 { log << "Line " << t.getLine() << ", '>' takes 2 arguments.\n"; return false; }
	
	auto list = t.children();
	if (! matchNumber (*list[0]))            { log << "Line " << list[1]->getLine() << ", expecting a number here.\n"; return false; }
	if (! matchElementExpr (*list[1]))       { log << "Line " << list[2]->getLine() << ", expecting an element spec here.\n"; return false; }
	return true;
}
bool SchemaValidator:: matchEqQuantifier (const se::Node& t) {
	if (t != "=")                            { return false; }
	if (t.children().size() != 2) 	 		 { log << "Line " << t.getLine() << ", '=' takes 2 arguments.\n"; return false; }
	
	auto list = t.children();
	if (! matchNumber (*list[0]))            { log << "Line " << list[1]->getLine() << ", expecting a number here.\n"; return false; }
	if (! matchElementExpr (*list[1]))       { log << "Line " << list[2]->getLine() << ", expecting an element spec here.\n"; return false; }
	return true;
}

bool SchemaValidator:: matchAnd (const se::Node& t) {
	if (t != "&")                               { return false; }
	if (t.children().empty())       			{ log << "Line " << t.getLine() << ", '&' takes atleast 1 argument.\n"; return false; }
	if (! matchSomeElementExpr (t.children()))  { log << "Line " << t.getLine() << ", expecting a sequence here.\n"; return false; }
	return true;
}

bool SchemaValidator:: matchOr (const se::Node& t) {
	if (t != "|")                              { return false; }
	if (t.children().empty())      			   { log << "Line " << t.getLine() << ", '|' takes atleast 1 argument.\n"; return false; }
	if (! matchSomeElementExpr (t.children())) { log << "Line " << t.getLine() << ", expecting a sequence here.\n"; return false; }
	return true;
}


bool SchemaValidator:: matchElementExpr (const se::Node& t) {
	return (matchMany (t) || matchSome (t) || matchAnd (t) || matchOr (t) ||
	        matchLtQuantifier (t) || matchGtQuantifier (t) || matchEqQuantifier (t) ||
			matchListElementExpr (t) || matchText (t));
}

bool SchemaValidator:: matchListElementExpr (const se::Node& t) {
	if (!t.isList()) return false;
	for (auto i : t.children()) {
		if (! matchElementExpr (*i)) { log << "Line " << t.getLine() << ", expecting a list here.\n"; return false; }
	}
	return true;
}

bool SchemaValidator:: matchSomeElementExpr (const se::Nodes_t& items) {
	if (items.empty()) return false;  // need at least one element
	
	for (auto i : items) {
		if (! matchElementExpr (*i)) return false;
	}
	return true;
}

bool SchemaValidator:: matchHas (const se::Node& t) {
	if (t != "has")                         { return false; }
	if (t.children().size() != 1) 			{ log << "Line " << t.getLine() << ", has takes 1 argument.\n"; return false; }
	
	auto list = t.children();
//	if (! matchListElementExpr (*list[0]))  { log << "Line " << t.getLine() << ", expecting a list here.\n"; return false; }
	if (! matchElementExpr (*list[0]))      { log << "Line " << t.getLine() << ", expecting an expression here.\n"; return false; }
	return true;
}

bool SchemaValidator:: matchIs (const se::Node& t) {
	if (t != "is")                        { return false; }
	if (t.children().size() != 1) 		  { log << "Line " << t.getLine() << ", is takes 1 argument.\n"; return false; }
	
	auto list = t.children();
	if (! matchText (*list[0]))            { log << "Line " << t.getLine() << ", expecting identifier here.\n"; return false; }
	return true;
}

bool SchemaValidator:: matchElement (const se::Node& t) {
	if (t != "element")                   { return false; }
	if (t.children().size() != 2) 		  { log << "Line " << t.getLine() << ", element takes 2 arguments.\n"; return false; }
	
	auto list = t.children();
	if (! matchText (*list[0]))            { log << "Line " << t.getLine() << "Expecting text after element.\n"; return false; }
	return matchHas (*list[1]);
}

bool SchemaValidator:: matchEnum (const se::Node& t) {
	if (t != "enum")                      { return false; }
	if (t.children().size() != 2) 		  { log << "Line " << t.getLine() << ", enum takes 2 arguments.\n"; return false; }
	
	auto list = t.children();
	if (! matchText (*list[0]))           { log << "Line " << t.getLine() << "Expecting text after enum.\n"; return false; }
	return matchListText (*list[1]);
}

bool SchemaValidator:: matchClass (const se::Node& t) {
	if (t != "class")                     { return false; }
	if (t.children().size() != 2) 		  { log << "Line " << t.getLine() << ", class takes 2 arguments.\n"; return false; }
	
	auto list = t.children();
	if (!matchText (*list[0]))           { log << "Line " << t.getLine() << "Expecting text after class.\n"; return false; }
	return matchListText (*list[1]);
}

bool SchemaValidator:: matchType (const se::Node& t) {
	if (t != "type")                      { return false; }
	if (t.children().size() != 2) 		  { log << "Line " << t.getLine() << ", type takes 2 arguments.\n"; return false; }
	
	auto list = t.children();
	if (! matchText (*list[0]))             { log << "Line " << t.getLine() << "Expecting text after type.\n"; return false; }
	return matchIs (*list[1]);
}

bool SchemaValidator:: matchImport (const se::Node& t) {
	if (t != "import")                    { return false; }
	if (t.children().size() != 1) 		  { log << "Line " << t.getLine() << ", import takes 1 argument.\n"; return false; }
	
	auto list = t.children();
	if (! matchText (*list[0]))           { log << "Line " << t.getLine() << "Expecting text after import.\n"; return false; }
	return true;
}

bool SchemaValidator:: matchMember (const se::Node& t) {
	return (matchElement (t) || matchEnum (t) || matchClass (t) || matchType (t) || matchImport (t));
}

bool SchemaValidator:: matchSomeMembers (const se::Nodes_t& items) {
	if (items.empty()) return false;  // need at least one element
	
	for (auto i : items) {
		if (! matchMember (*i)) return false;
	}
	return true;
}

bool SchemaValidator:: matchSchema (const se::Node& t) {
	if (t != "schema")                     { return false; }
	if (t.children().empty())              { log << "Line " << t.getLine() << ", schema takes atleast 1 argument.\n"; return false; }
	
	if (! matchSomeMembers (t.children())) { log << "Line " << t.getLine() << ", expecting schema members here.\n"; return false; }
	return true;
}

SchemaValidator:: SchemaValidator (ostream& alog) : log (alog) {
}

SchemaValidator:: ~SchemaValidator () {
}

bool SchemaValidator:: isValid (const se::Node& t) {
	return matchSchema (t);
}


