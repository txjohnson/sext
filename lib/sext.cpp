//
//  sext.cpp
//  cp
//
//  Created by Theo Johnson on 9/13/12.
//  Copyright (c) 2012 Theo Johnson. All rights reserved.
//

#include <sstream>
#include "sext.h"
using namespace std;

/*/// =========================================================================================================================================
/*/// -----------------------------------------------------------------------------------------------------------------------------------------
namespace se {

/*/// =========================================================================================================================================
	[RANGE]
	Text (data) is stored as ranges of bytes. For things that are actually text, we assume UTF-8 encoding.
	
	Rangepools are used as an arena for allocating and freeing ranges of bytes. Right now, all it does is use the C++
	allocator.
/*/// -----------------------------------------------------------------------------------------------------------------------------------------
	int rangecompare (const range& left, const range& right) {
		auto l = left.beg; auto r = right.beg;
		
		while (l != left.end) {
			if (*l < *r) return -1;
			if (*l > *r || r == right.end) return 1;
			++l; ++r;
		}
		if (r == right.end) return 0;
		return -1;
	}

	long   range:: asInteger () const {
		char digits[33];
		const size_t len = (end - beg) > 32 ? 32 : (end-beg);
		std::copy (beg, beg + len, digits);
		digits[len] = '\0';
		return std::atol (digits);
	}
	
	double range:: asReal () const {
		char digits[65];
		const size_t len = (end - beg) > 64 ? 64 : (end-beg);
		std::copy (beg, beg + len, digits);
		digits[len] = '\0';
		return std::atof (digits);
	}

	ostream& operator<< (ostream& out, const range& r) {
		for (auto i = r.beg; i != r.end; ++i) out.put (*i);
		return out;
	}

	rangepool:: rangepool () { }
	rangepool:: ~rangepool () { }
	
	range rangepool:: allocatefrom (uint8_t* ptr, size_t size) {
		auto m = new uint8_t [size];
		memcpy (m, ptr, size);
		return range (m, m + size);
	}
	
	range rangepool:: allocate (size_t size) {
		auto m = new uint8_t [size];
		return range (m, m + size);
	}
	
/*/// =========================================================================================================================================
	[SELECTION]
	
/*/// -----------------------------------------------------------------------------------------------------------------------------------------
	const Slice all   (Slice::all,   0, 0);
	const Slice first (Slice::first, 1, 0);
	const Slice last  (Slice::last,  1, 0);
	const Any   any;
	
	Slice:: Slice () : kind (Slice::all), count (0), start (0) { }
	Slice:: Slice (kind_e k, size_t c, size_t s) : kind (k), count (c), start (s) { }
	Slice:: ~Slice () { }

	NilNode Selection::nil;

	Selection:: Selection () { }
	Selection:: Selection (const Node& n) : initial (&n)  { }
	Selection:: ~Selection () { }

	Selection& Selection:: operator[] (const Any& a) {
		predicate_t f = [=](const Node& n) ->bool {
			return true;
		};
		
		select_using (f);
		return *this;
	}

	Selection& Selection:: operator[] (const range& r) {
		predicate_t f = [=](const Node& n) ->bool {
			if (n.getBytes() == r) return true;
			return false;
		};
		
		select_using (f);
		return *this;
	}

	Selection& Selection::operator[] (const Slice& s) {
		size_t lower, upper;
		
		size_t e = nodes.size();
		for (size_t i = 0; i != e; ++i) {
			auto j = nodes [i] ->children ();
			switch (s.kind) {
			case Slice::all:      lower = 0; upper = j.size(); break;
			case Slice::first:    lower = 0; upper = s.count; break;
			case Slice::last:     lower = j.size() - s.count; upper = j.size(); break;
			case Slice::butfirst: lower = s.count; upper = j.size(); break;
			case Slice::butlast:  lower = 0; upper = j.size() - s.count; break;
			case Slice::from:     lower = s.start; upper = s.start + s.count;
			}

			if (lower < j.size() && upper <= j.size()) {
				for (auto m = lower; m != upper; ++m) { nodes .push_back (j[m]); }
			}
		}
		nodes.erase (nodes.begin(), nodes.begin() + e);
		return *this;
	}


	Selection& Selection:: operator() (const Any& a) {
		predicate_t f = [=](const Node& n) ->bool {
			return true;
		};
		
		select_using (f);
		return *this;
	}

	Selection& Selection:: operator() (const range& r) {
		predicate_t f = [=](const Node& n) -> bool {
			if (n.getBytes() == r) return true;
			return false;
		};
		
		filter_using (f);
		return *this;
	}

	Selection& Selection::operator() (const Slice& s) {
		size_t lower, upper;
		
		switch (s.kind) {
		case Slice::all:      lower = 0; upper = nodes.size(); break;
		case Slice::first:    lower = 0; upper = s.count; break;
		case Slice::last:     lower = nodes.size() - s.count; upper = nodes.size(); break;
		case Slice::butfirst: lower = s.count; upper = nodes.size(); break;
		case Slice::butlast:  lower = 0; upper = nodes.size() - s.count; break;
		case Slice::from:     lower = s.start; upper = s.start + s.count;
		}

		if (lower < nodes.size() && upper <= nodes.size()) {
			for (auto i = 0; i != nodes.size(); ++i) {
				if (i < lower || i >= upper) nodes[i] = 0;
			}
			compact();
		}
		return *this;
	}

	void Selection:: compact () {
		auto j = nodes.begin();
		for (auto i = nodes.begin(); i != nodes.end(); ++i) {
			if (*i) {
				if   (i == j) { ++j; }
				else          { *j = *i; ++j; }
			}
		}
	}

	void Selection:: select_using (predicate_t& p) {
//		Selection s;
		
		if (initial) {
//			for (auto i : initial->children()) {
				if (p(*initial)) nodes .push_back (initial);
//			}
			initial = 0;
		}
		else {
			size_t e = nodes.size();
			for (size_t i = 0; i != e; ++i) {
				for (auto j : nodes[i]->children()) {
					cout << *j; cout << endl;
					if (p(*j)) nodes .push_back (j);
				}
			}
			nodes.erase (nodes.begin(), nodes.begin() + e);
		}
	}

	void Selection:: filter_using (predicate_t& p) {
		if (initial) {
			if (p (*initial)) nodes .push_back (initial);
			initial = 0;
		}
		else {
			if (nodes.empty()) return;
			for (auto i = nodes.begin(); i != nodes.end(); ++i) {
				if (!p (**i)) *i = 0;
			}
			compact ();
		}
	}


/*/// =========================================================================================================================================
	[NODE]
	The basic node provides all the methods for navigation and search. It's mean to be specialized for the type of
	data stored within.
/*/// -----------------------------------------------------------------------------------------------------------------------------------------
	Nodes_t nochildren;
	NilNode Node::nil;

	Node:: Node () : up (0) { }
	Node:: Node (const range& arange, size_t aline) : bytes (arange), line (aline), up (0) { }
	Node:: ~Node () { }

	bool  Node:: isNil ()      const { return false; }
	bool  Node:: isList ()     const { return false; }
	const Nodes_t& Node:: children () const { return nochildren; }
	const Node& Node:: operator[] (size_t index) const { return Node::nil; }

	void Node:: write (std::ostream& out) const {
		out << bytes;
	}


	List:: List (size_t aline)        { line = aline; }
	List:: ~List ()                   { for (auto a : kids) delete a; }
	bool   List:: isList ()           const { return true; }
	const Nodes_t& List:: children () const { return kids; }
	const Node& List:: operator[] (size_t index) const {
		if (index >= kids.size()) return Node::nil;
		return *kids[index];
	}

	void List:: write (std::ostream& out) const {
		out << "(";
		out << bytes;
		
		for (auto i : kids) {
			out << (" ");
			i ->write (out);
		}
		out << ")";
	}


	NilNode:: NilNode () {
		up = this;
	}
	
	NilNode:: ~NilNode () { }
	
	// Access
	bool   NilNode:: isNil ()      const { return true; }
	bool   NilNode:: isList ()     const { return false; }

	void NilNode:: write (std::ostream& out) const {
		out << "nil";
	}

/*/// =========================================================================================================================================
	[LEXER]
/*/// -----------------------------------------------------------------------------------------------------------------------------------------
	const char MASKBITS                = 0x3F;
	const char MASKBYTE                = 0x80;
	const char MASK2BYTES              = 0xC0;
	const char MASK3BYTES              = 0xE0;
	const char MASK4BYTES              = 0xF0;
	const char MASK5BYTES              = 0xF8;
	const char MASK6BYTES              = 0xFC;


	utf8reader:: utf8reader (istream& aninput) : in (aninput), code (0) { }
	utf8reader:: ~utf8reader () { }
	
	bool utf8reader:: get () {
		bytes .clear ();
		if (eof())                          { code = 0; return false; }

		char32_t t = in.get();

		// 0xxxxxxx
		if (t < MASKBYTE)                   { code = t; bytes .push_back (t); return false; }
		
		// 110xxxxx 10xxxxxx
		if ((t & MASK2BYTES) == MASK2BYTES) { code = ((t & 0x1F) << 6);  goto byte1; }

		// 1110xxxx 10xxxxxx 10xxxxxx
		if ((t & MASK3BYTES) == MASK3BYTES) { code = ((t & 0x0F) << 12); goto byte2; }

		// 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
		if ((t & MASK4BYTES) == MASK4BYTES) { code = ((t & 0x07) << 18); goto byte3; }
		
		// 111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
		if ((t & MASK5BYTES) == MASK5BYTES) { code = ((t & 0x03) << 24); goto byte4; }
		
		// 1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
		if ((t & MASK6BYTES) == MASK6BYTES)	{ code = ((t & 0x01) << 30); goto byte5; }
		return false;
		
		byte5: bytes .push_back (t = in.get()); code |= (t & MASKBITS) << 24;
		byte4: bytes .push_back (t = in.get()); code |= (t & MASKBITS) << 18;
		byte3: bytes .push_back (t = in.get()); code |= (t & MASKBITS) << 12;
		byte2: bytes .push_back (t = in.get()); code |= (t & MASKBITS) << 6;
		byte1: bytes .push_back (t = in.get()); code |=  t & MASKBITS;
		return true;
	}
	
	bool utf8reader::getbyte () {
		bytes.clear();
		if (in.eof()) { code = 0; return false; }
		code = in.get ();
		bytes.push_back (code);
		return true;
	}

	bool accept_for_words (utf8reader& src) {
		if (src.code == '(' || src.code == ')' || src.code == '{' || src.code == '}'
			|| src.code == '\'' || isspace (src.code) || src.eof()) return false;
		if (src.code == '\\') src .get ();
		return true;
	}

	bool accept_for_quoted_words (utf8reader& src) {
		if (src.code == '}' || src.eof()) return false;
		if (src.code == '\\') src .get ();
		return true;
	}
	
	
	token:: token () { }
	token:: token (tokentype atype, const range& somebytes, size_t aline) : type (atype), bytes (somebytes), line (aline) { }

	Tokenizer:: Tokenizer (utf8reader& asource, rangepool& apool) : src (asource), pool (apool), line (1)  {
		src.get(); // move to the first character
	}
	
	Tokenizer:: ~Tokenizer () { }
	
	bool Tokenizer:: advance () {
	restart:
		if (src.eof()) { return false; current.type = tokentype::unknown; }
		
		while (isspace (src.code) && !src.eof()) {
			if (src.code == '\n') ++line;
			src.get ();
		}

		begin_accumulating ();
		if (src.code == '\'') { src.get ();
								if (src.code == '(')  { current = token (tokentype::qlparen, range(), line); src .get(); return true; }
								goto restart; }
		if (src.code == '(')  { current = token (tokentype::lparen, range(), line); src .get(); return true; }
		if (src.code == ')')  { current = token (tokentype::rparen, range(), line); src .get(); return true; }
		if (src.code == '{')  {
			src.get ();
			if (src.code == '|') {
				src.get();
				auto len = get_number ();
				if (src.code == '|') src.get();
				else {
					// report error
				}
				uint8_t* buf = accum;
				range r = (len <= MaxWordSize) ? range (accum, accum + len) : pool.allocate (len);
				read_bytes_into (const_cast<uint8_t*>(r.beg), len);
				current = token (tokentype::string, r, line);
				if (src.code == '}') src.get ();
				else {
					// report error
				}
			}
			else {
				current = token (tokentype::string, read_to_delimiter (accept_for_quoted_words), line);
				if (src.code == '}') src.get ();
				else {
					// report error
				}
			}
			return true;
		}
		
		return_word:
		current = token (tokentype::word, read_to_delimiter (accept_for_words), line);
		return true;
	}

	range Tokenizer:: read_to_delimiter (acceptor_t accept) {
		while ((*accept)(src)) {
			if (!accumulate (src.bytes)) {
				src.seek(cpoint);
				auto end = find_next_delimiter (accept);
				range r = pool.allocate (end - cpoint);
				src.seek(cpoint);
				src.get();
				read_chars_until (const_cast<uint8_t*>(r.beg), end, accept);
				return r;
			}
			src.get();
		}
		return pool.allocatefrom (accum, used);
	}

	size_t Tokenizer:: find_next_delimiter (acceptor_t accept) {
		while ((*accept)(src)) src.get();
		return src.tell ();
	}
	
	void Tokenizer:: read_bytes_into (uint8_t* buf, size_t howmany) {
		while (howmany > 0) { *buf = src.code; ++buf; --howmany; src.get();	}
	}

	void Tokenizer:: read_chars_until (uint8_t* buf, size_t position, acceptor_t accept) {
		while (src.tell() < position) {
			(*accept)(src);

			for (auto i : src.bytes) { *buf = i; ++buf; }
			src.get();
		}
	}

	size_t Tokenizer:: get_number () {
		size_t num = 0;
		while (isdigit (src.code)) {
			num *= 10; num += (src.code - 48);
			src.get();
		}
		return num;
	}
	

/*/// =========================================================================================================================================
	[PARSER]
/*/// -----------------------------------------------------------------------------------------------------------------------------------------
	Parser:: Parser (Tokenizer& atokenizer) : tokens (atokenizer) { }
	Parser:: ~Parser () { }
	
	Node* Parser:: get () {
		if (!tokens .advance()) return 0;
		
		if (tokens.current.type == tokentype::word)        { return new Node (tokens.current.bytes, tokens.current.line); }
		else if (tokens.current.type == tokentype::string) { return new Node (tokens.current.bytes, tokens.current.line); }
		else if (tokens.current.type == tokentype::lparen || tokens.current.type == tokentype::qlparen) {
			List* list = new List (tokens.current.line);
			if (tokens.current.type == tokentype::lparen) {
				tokens .advance();
				list->bytes = tokens.current.bytes;
			}
			else {
				list->bytes = "__list__";
			}
			for (Node* n = get (); n != 0; n = get ()) { list ->kids.push_back (n); }
			
			if (tokens.current.type == tokentype::rparen) {
				return list;
			}
			else {
				// badly formed list. error
				if (list) delete list;
			}
		}
		return 0;
	}

/*/// =========================================================================================================================================
	[MATCHING]
	Here, we develop the rule machinery that allows us to make matches and generate output. XSLT uses a soup of rules, but we don't
	do that here. 
	
	Subclass pattern to create own patterns and actions.
/*/// -----------------------------------------------------------------------------------------------------------------------------------------

	RuleSet:: RuleSet ()  { }
	RuleSet:: RuleSet (std::initializer_list<rule_t> l) : rules (l) { }

	RuleSet:: ~RuleSet () { }
	
	bool RuleSet:: apply_to (const Node& n) {
		for (auto r : rules) {
			if (r (n, *this)) return true;
		}
		return false;
	}

	void RuleSet:: apply_to_children (const Node& n) {
		for (auto j : n.children()) {
			apply_to (*j);
		}
	}

	void RuleSet:: apply_to_selection (const Selection & s) {
		if (s.nodes.empty()) return;
		
		for (auto i : s.nodes) { apply_to (*i); }
	}
	
	void RuleSet:: apply_to_children (const Selection & s) {
		if (s.nodes.empty()) return;
		
		for (auto i : s.nodes) {
			for (auto j : i->children()) {
				apply_to (*j);
			}
		}
	}

	void RuleSet:: apply_to_list (const Nodes_t & s) {
		if (s.empty()) return;
		
		for (auto i : s) {
			apply_to (*i);
		}
	}

	void RuleSet:: apply_and_gather (const Node& n, RuleSet& use, strings_t& into) {
		stringstream ss;
		RedirectStream (cout, ss);
		use.apply_to (n);
		into .push_back (ss.str());
	}

	void RuleSet:: apply_and_gather (const Nodes_t& n,   RuleSet& use, strings_t& into) {
		for (auto i : n) {
			stringstream ss;
			RedirectStream (cout, ss);
			use.apply_to (*i);
			into .push_back (ss.str());
		}
	}
	
	RuleSetwDefault:: RuleSetwDefault () { }
	RuleSetwDefault:: RuleSetwDefault (std::initializer_list<rule_t> l) : RuleSet (l) { }
	RuleSetwDefault:: ~RuleSetwDefault () { }
	
	bool RuleSetwDefault:: apply_to (const Node& n) {
		if (!RuleSet::apply_to(n)) {
			cout << n;
		}
		return true;
	}
	
	void RuleSetwDefault:: apply_to_selection (const Selection & s) {
		if (rules.empty()) {
			for (auto i : s.nodes) { cout << *i; }
			return;
		}
		
		RuleSet::apply_to_selection (s);
	}

	void RuleSetwDefault:: apply_to_children (const Selection & s) {
		if (rules.empty()) {
			for (auto i : s.nodes) { cout << *i; }
			return;
		}
		
		RuleSet::apply_to_children (s);
	}

	void RuleSetwDefault:: apply_to_list (const Nodes_t & s) {
		if (rules.empty()) {
			for (auto i : s) { cout << *i; }
			return;
		}
		
		RuleSet::apply_to_list (s);
	}

/*/// =========================================================================================================================================
	[STARTUP]
	Before using the framework, call the initialization procedure. When done, call the tear down procedure.
/*/// -----------------------------------------------------------------------------------------------------------------------------------------
	void initialize_sext () {
	}
	
	void cleanup_sext () {
	}

	
/*/// =========================================================================================================================================
/*/// -----------------------------------------------------------------------------------------------------------------------------------------

}
