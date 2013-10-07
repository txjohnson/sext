//
//  main.cpp
//  validateschema
//
//  Created by Theo Johnson on 10/1/13.
//  Copyright (c) 2013 Theo Johnson. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <sstream>
#include "sext.h"
#include "validate.h"
using namespace std;


/*/// =========================================================================================================================================
	[SHARED]
	Variables shared by each of the rule sets below.
/*/// -----------------------------------------------------------------------------------------------------------------------------------------
string        classname = "Validator";
string        suffix;
string        infix;
se::range     root;
se::strings_t srctxt;
se::strings_t hdrtxt;
int           parm_no = 0;
int           parm_mode = 0; // 0 = exact, 1 at least

/*/// =========================================================================================================================================
	[START]
	Find the start rule.
/*/// -----------------------------------------------------------------------------------------------------------------------------------------
typedef std::pair <se::range, size_t> occurrence_t;

bool operator< (const occurrence_t& o1, const occurrence_t& o2) { return o1.second < o2.second; }

std::vector <occurrence_t> occurrences;

occurrence_t* findocc (const se::range& r) {
	for (occurrence_t& i : occurrences) {
		if (i.first == r) return &i;
	}
	return 0;
}

def_rule_set(prepare)
	when   ["schema"]   execute { self.apply_to_children (matched); } done,
	when   ["element"]  execute { occurrences .push_back (occurrence_t (node[0].getBytes(), 0)); self.apply_to_children (matched); } done,
end_rule_set

def_rule_set(start)
	when   ["schema"]   execute { occurrences.clear(); prepare.apply_to (node); self.apply_to_children (matched); } done,
	when   ["element"]  execute { self.apply_to_children (matched); } done,
	when   ["class"]    execute { self.apply_to_children (matched); } done,
	when   ["has"]      execute { self.apply_to_children (matched); } done,
	when   ["many"]     execute { self.apply_to_children (matched); } done,
	when   ["some"]     execute { self.apply_to_children (matched); } done,
	when   ["maybe"]    execute { self.apply_to_children (matched); } done,
	when   ["__list__"] execute { self.apply_to_children (matched); } done,
	when   ["&"]        execute { self.apply_to_children (matched); } done,
	when   ["|"]        execute { self.apply_to_children (matched); } done,
	when   ["="]        execute { self.apply_to_children (matched); } done,
	when   ["<"]        execute { self.apply_to_children (matched); } done,
	when   [">"]        execute { self.apply_to_children (matched); } done,
	when   [se::any]    execute { auto i = findocc (node.getBytes()); if (i) ++(i->second); } done
end_rule_set

/*/// =========================================================================================================================================
	[HEADER]
	These rules generate the header file for a validator.
/*/// -----------------------------------------------------------------------------------------------------------------------------------------
def_rule_set(header)
	when    ["schema"]
	execute {	cout << "#ifndef __" << classname << "_header__\n";
				cout << "#define __" << classname << "_header__\n\n";
				cout << "#include \"sext.h\"\n\n";
				cout << "class " << classname << "{\n";
				cout << "public: \n";
				cout << "   " << classname << " (ostream& alog);\n\t~" << classname << " ();\n";
				cout << "   bool isValid (const se::Node& t);\nprivate:\n";
				cout << "   ostream& log;\n";
				for (auto i : hdrtxt) {
					cout << i;
				}
				cout << "};\n";
				cout << "#endif\n";
			}
	done
end_rule_set




/*/// =========================================================================================================================================
	[SOURCE]
	These rules generate the source file for a validator.
/*/// -----------------------------------------------------------------------------------------------------------------------------------------
def_rule_set(source)
	when    ["schema"]
	execute {	cout << "#include \"" << classname << ".h\"\n\n\n";
				cout << classname << ":: " << classname << " (ostream& alog) : log (alog) { }\n";
				cout << classname << ":: ~" << classname << " () { }\n";
				cout << "bool " << classname << ":: isValid (const se::Node& t) {\n";
				cout << "   Nodes_t root;\n";
				cout << "   root .push_back (&n);\n";
				cout << "   auto nr = NRange_t (root.begin(), root.end());\n";
				cout << "   return match_" << root << " (nr);\n";
				cout << "}\n\n";
				srctxt .clear();
				suffix .clear ();
				self.apply_to_children (node);
				for (auto i : srctxt) {
					cout << i;
				}
			}
	done,
	when    ["has"]
	execute {
				self.apply_to_children (matched);
			}
	done,
	when    ["is"]
	execute {
				self.apply_to_children (matched);
			}
	done,
	when    ["element"]
	execute { 	cout << "bool " << classname << ":: " << "match_" << node[0].getBytes() << " (se::NRange_t& anr) {\n";
				cout << "   if (anr.beg == anr.end) return false;\n";
				cout << "   Node& n = *anr.beg;\n";
				cout << "   if (n != \"" << node[0].getBytes() << "\")  return false;\n";
				cout << "   auto nr = NRange_t (n.children().begin(), n.children().end());\n";
				cout << "   if (";
				infix.clear(); suffix.clear();
				se::append_to_string (infix, node[0].getBytes());
				self.apply_to_children (node[1]);
				cout << ") { ++anr.beg; return true; }\n";
				cout << "   log << \"(Line: \" << n.getLine() << \") " << node[0].getBytes() << " is badly formed.\\n\"; }\n";
				cout << "   return false;\n";
				cout << "}\n\n";
				{	std::stringstream ss;
					se::RedirectStream rs (cout, ss);
					cout << "   bool match_" << node[0].getBytes() << " (se::NRange_t& nr);\n";
					hdrtxt .push_back (ss.str());
				}
			}
	done,
	when    ["class"]
	execute { 	cout << "bool " << classname << ":: " << "match_" << node[0].getBytes() << " (se::NRange_t& nr) {\n";
				cout << "   if (nr.first == nr.second) return true;\n";
				cout << "   return (";
				bool next = false;
				for (auto i : node.children()) {
					if (next) cout << " || "; else next = true;
					self.apply_to (*i);
				}
				cout << ");\n";
				cout << "}\n\n";
				{	std::stringstream ss;
					se::RedirectStream rs (cout, ss);
					cout << "   bool match_" << node[0].getBytes() << " (se::NRange_t& nr);\n";
					hdrtxt .push_back (ss.str());
				}
			}
	done,
	when    ["type"]
	execute { 	cout << "bool " << classname << ":: " << "match_" << node[0].getBytes() << " (se::NRange_t& nr) {\n";
				cout << "   return (";
				self .apply_to (node[1]);
				cout << ");\n";
				cout << "}\n\n";
				{	std::stringstream ss;
					se::RedirectStream rs (cout, ss);
					cout << "   bool match_" << node[0].getBytes() << " (se::NRange_t& nr);\n";
					hdrtxt .push_back (ss.str());
				}
			}
	done,
	when    ["enum"]
	execute { 	cout << "bool " << classname << ":: " << "match_" << node[0].getBytes() << " (se::NRange_t& nr) {\n";
				cout << "   if (nr.first == nr.second) return true;\n";
				cout << "   if (";
				bool next = false;
				for (auto i : node[1].children()) {
					if (next) cout << " || "; else next = true;
					cout << "((*nr.beg)->getBytes() == \"";
					cout << i->getBytes() << "\")";
				}
				cout << ") { ++nr.beg; return true; };\n";
				cout << "   return false;\n";
				cout << "}\n\n";
				{	std::stringstream ss;
					se::RedirectStream rs (cout, ss);
					cout << "   bool match_" << node[0].getBytes() << " (se::NRange_t& nr);\n";
					hdrtxt .push_back (ss.str());
				}
			}
	done,

	when    ["many"]
	execute {	suffix .push_back ('a');
				cout << "match_" << infix << "many" << suffix << " (nr)";
				{	std::stringstream ss;
					se::RedirectStream rs (cout, ss);
					cout << "bool " << classname << ":: " << "match_" << infix << "many" << suffix << " (se::NRange_t& nr) {\n";
					cout << "   if (nr.first == nr.second) return true;\n";
					cout << "   auto old = nr;\n";
					cout << "   while (true) {\n   told = nr;\n";
					cout << "      if (! "; self.apply_to (node[0]); cout << ") { nr = old; break; }\n";
					cout << "   }\n   return true;\n";
					cout << "}\n\n";
					srctxt .push_back (ss.str());
				}
				{	std::stringstream ss;
					se::RedirectStream rs (cout, ss);
					cout << "   bool match_" << infix << "many" << suffix << " (se::NRange_t& nr);\n";
					hdrtxt .push_back (ss.str());
				}
				suffix.pop_back();
			}
	done,
	when    ["some"]
	execute { 	suffix .push_back ('b');
				cout << "match_" << infix << "some" << suffix << " (nr)";
				{	std::stringstream ss;
					se::RedirectStream rs (cout, ss);
					cout << "bool " << classname << ":: " << "match_" << infix << "some" << suffix << " (se::NRange_t& nr) {\n";
					cout << "   if (nr.first == nr.second) return false;\n";
					cout << "   auto old = nr;\nauto i = 0;\n";
					cout << "   while (true) {\n   told = nr;\n";
					cout << "      if ("; self.apply_to (node[0]); cout << ") { ++i; }\n";
					cout << "   }\n   if (i) return true;\n";
					cout << "   nr = old;\n";
					cout << "   return false;\n";
					cout << "}\n\n";
					srctxt .push_back (ss.str());
				}
				{	std::stringstream ss;
					se::RedirectStream rs (cout, ss);
					cout << "   bool match_" << infix << "some" << suffix << " (se::NRange_t& nr);\n";
					hdrtxt .push_back (ss.str());
				}
				suffix.pop_back();
			}
	done,
	when    ["maybe"]
	execute { 	suffix .push_back ('c');
				cout << "match_" << infix << "maybe" << suffix << " (nr)";
				{	std::stringstream ss;
					se::RedirectStream rs (cout, ss);
					cout << "bool " << classname << ":: " << "match_" << infix << "maybe" << suffix << " (se::NRange_t& nr) {\n";
					cout << "   if (nr.first == nr.second) return true;\n";
					cout << "   if ("; self.apply_to (*node.children()[0]); cout << ") { return true; }\n";
					cout << "   return false;\n";
					cout << "}\n\n";
					srctxt .push_back (ss.str());
				}
				{	std::stringstream ss;
					se::RedirectStream rs (cout, ss);
					cout << "   bool match_" << infix << "maybe" << suffix << " (se::NRange_t& nr);\n";
					hdrtxt .push_back (ss.str());
				}
				suffix.pop_back();
			}
	done,
	when    ["="]
	execute { 	suffix .push_back ('d');
				cout << "match_" << infix << "exactly" << suffix << " (nr)";
				{	std::stringstream ss;
					se::RedirectStream rs (cout, ss);
					cout << "bool " << classname << ":: " << "match_" << infix << "exactly" << suffix << " (se::NRange_t& nr) {\n";
					cout << "   if (nr.first == nr.second) return false;\n";
					cout << "   auto old = nr;\n";
					cout << "   for (int i = 0; i != " << node[0].getBytes() << "; ++i) {\n";
					cout << "      if (! "; self.apply_to (node[1]); cout << ") { nr = old; return false; }\n";
					cout << "   }\n";
					cout << "   return true;\n";
					cout << "}\n\n";
					srctxt .push_back (ss.str());
				}
				{	std::stringstream ss;
					se::RedirectStream rs (cout, ss);
					cout << "   bool match_" << infix << "exactly" << suffix << " (se::NRange_t& nr);\n";
					hdrtxt .push_back (ss.str());
				}
				suffix.pop_back();
			}
	done,
	when    ["<"]
	execute { 	suffix .push_back ('e');
				cout << "match_" << infix << "atmost" << suffix << " (nr)";
				{	std::stringstream ss;
					se::RedirectStream rs (cout, ss);
					cout << "bool " << classname << ":: " << "match_" << infix << "atmost" << suffix << " (se::NRange_t& nr) {\n";
					cout << "   if (nr.first == nr.second) return false;\n";
					cout << "   auto old = nr;\n";
					cout << "   int i = 0;\n";
					cout << "   while (i < " << node[0].getBytes() << ") {\n";
					cout << "      if ("; self.apply_to (node[1]); cout << ") { ++i; }\n";
					cout << "      else break;\n";
					cout << "   }\n";
					cout << "   if (i < " << node[0].getBytes() << ") return true;\n";
					cout << "   nr = old;\n";
					cout << "   return false;\n";
					cout << "}\n\n";
					srctxt .push_back (ss.str());
				}
				{	std::stringstream ss;
					se::RedirectStream rs (cout, ss);
					cout << "   bool match_" << infix << "atmost" << suffix << " (se::NRange_t& nr);\n";
					hdrtxt .push_back (ss.str());
				}
				suffix.pop_back();
			}
	done,
	when    [">"]
	execute { 	suffix .push_back ('f');
				cout << "match_" << infix << "atleast" << suffix << " (nr)";
				{	std::stringstream ss;
					se::RedirectStream rs (cout, ss);
					cout << "bool " << classname << ":: " << "match_" << infix << "atleast" << suffix << " (se::NRange_t& nr) {\n";
					cout << "   if (nr.first == nr.second) return false;\n";
					cout << "   auto old = nr;\n";
					cout << "   int i;\n";
					cout << "   for (i = 0; i != " << node[0].getBytes() << "; ++i) {\n";
					cout << "      if (! "; self.apply_to (node[1]); cout << ") { return break; }\n";
					cout << "   }\n";
					cout << "   if (i > " << node[0].getBytes() << ") { return true; }\n";
					cout << "   nr = old;\n";
					cout << "   return false;\n";
					cout << "}\n\n";
					srctxt .push_back (ss.str());
				}
				{	std::stringstream ss;
					se::RedirectStream rs (cout, ss);
					cout << "   bool match_" << infix << "atleast" << suffix << " (se::NRange_t& nr);\n";
					hdrtxt .push_back (ss.str());
				}
				suffix.pop_back();
			}
	done,
	when    ["&"]
	execute { 	suffix .push_back ('g');
				cout << "match_" << infix << "seq" << suffix << " (nr)";
				{	std::stringstream ss;
					se::RedirectStream rs (cout, ss);
					cout << "bool " << classname << ":: " << "match_" << infix << "seq" << suffix << " (se::NRange_t& nr) {\n";
					cout << "   if (nr.first == nr.second) return false;\n";
					cout << "   auto old = nr;\n";
					cout << "   if (";
					bool next = false;
					for (auto i : node.children()) {
						if (next) cout << " && "; else next = true;
						self.apply_to (*i);
					}
					cout << ") return true;\n";
					cout << "   nr = old;\n";
					cout << "   return false;\n";
					cout << "}\n\n";
					srctxt .push_back (ss.str());
				}
				{	std::stringstream ss;
					se::RedirectStream rs (cout, ss);
					cout << "   bool match_" << infix << "seq" << suffix << " (se::NRange_t& nr);\n";
					hdrtxt .push_back (ss.str());
				}
				suffix.pop_back();
			}
	done,
	when    ("|")
	execute { 	suffix .push_back ('h');
				cout << "match_" << infix << "alt" << suffix << " (nr)";
				{	std::stringstream ss;
					se::RedirectStream rs (cout, ss);
					cout << "bool " << classname << ":: " << "match_" << infix << "alt" << suffix << " (se::NRange_t& nr) {\n";
					cout << "   if (nr.first == nr.second) return true;\n";
					cout << "   return (";
					bool next = false;
					for (auto i : node.children()) {
						if (next) cout << " || "; else next = true;
						self.apply_to (*i);
					}
					cout << ");\n";
					cout << "}\n\n";
					srctxt .push_back (ss.str());
				}
				{	std::stringstream ss;
					se::RedirectStream rs (cout, ss);
					cout << "   bool match_" << infix << "alt" << suffix << " (se::NRange_t& nr);\n";
					hdrtxt .push_back (ss.str());
				}
				suffix.pop_back();
			}
	done,
	when    ["__list__"]
	execute { 	suffix .push_back ('l');
				cout << "match_" << infix << "list" << suffix << " (nr)";
				{	std::stringstream ss;
					se::RedirectStream rs (cout, ss);
					cout << "bool " << classname << ":: " << "match_" << infix << "list" << suffix << " (se::NRange_t& nr) {\n";
					cout << "   if (nr.first == nr.second) return false;\n";
					cout << "   auto old = nr;\n";
					cout << "   if (";
					bool next = false;
					for (auto i : node.children()) {
						if (next) cout << " && "; else next = true;
						self.apply_to (*i);
					}
					cout << ") return true;\n";
					cout << "   nr = old;\n";
					cout << "   return false;\n";
					cout << "}\n\n";
					srctxt .push_back (ss.str());
				}
				{	std::stringstream ss;
					se::RedirectStream rs (cout, ss);
					cout << "   bool match_" << infix << "list" << suffix << " (se::NRange_t& nr);\n";
					hdrtxt .push_back (ss.str());
				}
				suffix.pop_back();
			}
	done,
	when    [se::any]
	execute {   cout << "match_" << node.getBytes() <<  " (nr)";
	 		}
	done
end_rule_set

int main(int argc, const char * argv[])
{
	if (argc < 2) { cout << "No file to check was provided. Exiting.\n"; return 1; }

	ifstream in (argv[1]);
	if (in.fail()) { cout << "Unable to open file. Exiting.\n"; return 1; }
	
	se::rangepool   pool;
	se::utf8reader  reader (in);
	se::Tokenizer   tokens (reader, pool);
	se::Parser      parser (tokens);

	se::Node* n = parser .get ();
	if (!n) { cout << "Unable to parse file " << argv[1] << ". Exiting.\n"; return 1; }

	cout << *n;
	cout << endl << endl;
	
	SchemaValidator validator (std::cout);
	if (validator.isValid (*n)) {
		start .apply_to (*n);
		std::sort (occurrences.begin(), occurrences.end());

		cout << "Elements and number of times found:\n";
		for (auto i : occurrences) {
			cout << "   " << i.first << " = " << i.second << endl;
		}

		occurrences .erase (
			std::remove_if (occurrences.begin(), occurrences.end(), [](occurrence_t& i) { return i.second > 1; }),
			occurrences.end());
			
		if (occurrences.empty())    { cout << "Error. There is no root element in this schema.\n"; return 1; }
		if (occurrences.size() > 1) { cout << "Erorr. There are multiple candidates for root element in this schema: ";
									  for (auto i : occurrences) { cout << i.first << " "; }
									  cout << endl; return 1; }
	
		root = occurrences[0].first;
		cout << "Success! File is a valid schema definition\n";
	}
	else {
		cout << "Error. File is not a valid schema definition\n"; return 1;
	}
	
	
	if (argc == 3) {
		cout << "Creating source files using '" << argv[2] << "'...\n";
		classname = argv[2];
		string hfile (argv[2]);
		string sfile (argv[2]);
		hfile .append (".h");
		sfile .append (".cpp");
		ofstream hout (hfile.c_str());
		ofstream sout (sfile.c_str());
		
		if (hout.fail () || sout.fail ()) { cout << "Unable to create output files.\n"; return 1; }
		
		{ se::RedirectStream s (cout, sout); source .apply_to (*n); }
		{ se::RedirectStream h (cout, hout); header .apply_to (*n); }
	}
    return 0;
}

