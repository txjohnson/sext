#ifndef _sext_header
#define _sext_header
//
//  sext.h
//  cp
//
//  Created by Theo Johnson on 9/13/12.
//  Copyright (c) 2012 Theo Johnson. All rights reserved.
//

#include <iostream>
#include <vector>
#include <string>
//#include <map>
//#include <unordered_map>
#include <initializer_list>

#ifndef NDEBUG
# define require(X) assert(X)
#else
//# define require(X) if (!(X)) return 0
# define require(X)
#endif


/*/// =========================================================================================================================================
	TODO: 	Add interrogation to determine data type, ie. Integer, Real, Nat, Date, Time, RegEx
			Add IO manipulators for pretty printing s-expressions.
/*/// -----------------------------------------------------------------------------------------------------------------------------------------
namespace se {
/*/// =========================================================================================================================================
	[REDIRECT]
	Useful for redirecting streams via filebuffers. The library expects you to use it to redirect cout since everything
	uses that.
/*/// -----------------------------------------------------------------------------------------------------------------------------------------
	class RedirectStream {
	public:
		RedirectStream (std::ios& from, std::ios& to) : redirected(from), old (from.rdbuf()) { from.rdbuf(to.rdbuf()); }
		~RedirectStream () { redirected .rdbuf (old); }
	private:
		std::ios&       redirected;
		std::streambuf* old;
	};

/*/// =========================================================================================================================================
	[TYPES]
/*/// -----------------------------------------------------------------------------------------------------------------------------------------
	typedef std::vector <std::string> strings_t;
	
/*/// =========================================================================================================================================
	[RANGE]
	Text (data) is stored as ranges of bytes. When we're treating the bytes as text, its assumed to encoded as UTF-8.
	
	Rangepools are used as an arena for allocating and freeing ranges of bytes. Right now, all it does is use the C++
	allocator.
/*/// -----------------------------------------------------------------------------------------------------------------------------------------
	struct range {
		const uint8_t* beg;
		const uint8_t* end;
		
		range () : beg (0), end (0) { }
		range (const char* s) : beg ((uint8_t*)s), end ((uint8_t*)s + ::strlen(s)) { }
		range (const uint8_t* b, const uint8_t* e) : beg (b), end (e) { }

		inline bool isnil () const { return (beg == 0); }
		long   asInteger () const;
		double asReal () const;
	};

	int rangecompare (const range& left, const range& right);
	
	inline bool operator== (const range& left, const range& right) { return (rangecompare (left, right) == 0); }
	inline bool operator!= (const range& left, const range& right) { return (rangecompare (left, right) != 0); }
	inline bool operator<  (const range& left, const range& right) { return (rangecompare (left, right) <  0); }
	inline bool operator> (const range& left, const range& right)  { return (rangecompare (left, right)  > 0); }

	long range_to_integer (const range& r);
	double range_to_real (const range& r);

	inline bool operator==(const range& left, const char* right)   { return (left == range (right)); }
	inline bool operator!=(const range& left, const char* right)   { return (left != range (right)); }
	inline bool operator< (const range& left, const char* right)   { return (left <  range (right)); }
	inline bool operator> (const range& left, const char* right)   { return (left  > range (right)); }

	inline void append_to_string (std::string& s, const range& r) { for (auto i = r.beg; i != r.end; ++i) s.push_back (*i); }
	
	std::ostream& operator<< (std::ostream& out, const range& r);
	
	class rangepool {
	public:
		rangepool ();
		~rangepool ();
		
		range allocatefrom (uint8_t* ptr, size_t size);
		range allocate (size_t size);
		
	private:
		std::vector <range> ranges;
	};

/*/// =========================================================================================================================================
	[NODE]
	The basic node provides all the methods for navigation and search. It's mean to be specialized for the type of
	data stored within.
/*/// -----------------------------------------------------------------------------------------------------------------------------------------
	class Node;
	class NilNode;
	class Selection;
	typedef std::vector<const Node*> Nodes_t;
	typedef std::pair <Nodes_t::iterator, Nodes_t::iterator> NRange_t;
	
	class Node {
	protected:
		Node*  up;		    // parent
		range  bytes;		// text as obtained from stream
		size_t line;
		friend class Parser;
		friend class Selection;
		inline operator range& () { return bytes; }
		static NilNode nil;
	public:
		
		Node ();
		Node (const range& arange, size_t aline);
		virtual ~Node ();
		
	  	// Navigation
		inline Node& parent () const;
		virtual const Nodes_t& children ()   const;
		virtual const Node& operator[] (size_t index) const;
		
		// Access
		inline const range&  getBytes ()    const { return bytes; }
		inline size_t        getLine ()     const { return line; }
		
		// Interrogation
		virtual bool isNil () const;
		virtual bool isList () const;
		inline bool operator== (const char* s)  { return bytes == s; }
		inline bool operator== (const range& r) { return bytes == r; }
		inline bool operator!= (const char* s)  { return bytes != s; }
		inline bool operator!= (const range& r) { return bytes != r; }

		// output
		virtual void write (std::ostream& out) const;
		
		// conversion
		inline operator const range& () const { return bytes; }
	};
	
	class List : public Node {
	protected:
		Nodes_t  kids;		// children
		friend class Parser;
		friend class Selection;
		
	public:
		List ();
		List (size_t aline);
		virtual ~List ();

		virtual const Nodes_t& children () const;
		virtual const Node& operator[] (size_t index) const;

		virtual bool isList () const;

		virtual void write (std::ostream& out) const;
	};
	
	class NilNode : public Node {
	public:

		NilNode ();
		virtual ~NilNode ();
		
		virtual bool isNil () const;
		virtual bool isList () const;

		virtual void write (std::ostream& out) const;
	};

	inline Node& Node:: parent ()     const { if (up) return *up; return nil; }

	inline std::ostream& operator<< (std::ostream& out, const Node& n) { n.write (out); return out; }
	
/*/// =========================================================================================================================================
	[SELECTION]
	Selections are used to query the s-expression. Like XPath, its to be used to select parts of the s-expression on which to operate.
	Here we use a little template-fu to allow us to handle simple expressions.
/*/// -----------------------------------------------------------------------------------------------------------------------------------------
	class RuleSet;
	typedef std::function <bool(const Node&)> predicate_t;

	struct Slice {
		enum kind_e { all, first, last, butfirst, butlast, from };
		kind_e   kind;
		size_t   count;
		size_t   start;
		
		Slice ();
		Slice (kind_e k, size_t c, size_t s);
		~Slice ();
	};
	
	struct Any {
		Any () { }
	};
	
	extern const Slice all;
	extern const Slice first;
	extern const Slice last;
	extern const Any   any;
	inline Slice butfirst (size_t howmany) { return Slice (Slice::butfirst, howmany, 0); }
	inline Slice butlast  (size_t howmany) { return Slice (Slice::butlast,  howmany, 0); }
	
	class Selection {
	public:
		Selection ();
		explicit Selection (const Node& n);
		~Selection ();
		
		inline bool notempty ()   { return !nodes.empty(); }
		inline bool empty ()      { return nodes.empty(); }
		inline operator bool ()   { return !nodes.empty(); }

		inline const Node& first () const { if (nodes.empty()) { if (initial) return *initial; return nil; } return *nodes.front(); }
		inline const Node& last () const  { if (nodes.empty()) { if (initial) return *initial; return nil; } return *nodes.back (); }
		inline const Node& operator[] (int i) {
				if (nodes.empty()) { if (initial) return *initial; return nil; }
				if (i > nodes.size()) return nil;
				return *nodes.back (); }
		
		inline void add (Node* n) { if (n) nodes .push_back (n); }
		inline size_t size ()     { return nodes.size(); }
		
		inline Selection& operator[] (predicate_t& p)  { select_using (p); return *this; }
		inline Selection& operator[] (const char* s)   { range r (s); return this->operator[](r); };
		Selection& operator[] (const range& r);
		Selection& operator[] (const Slice& s);
		Selection& operator[] (const Any& a);
		
		inline Selection& operator() (predicate_t& p)  { filter_using (p); return *this; }
		Selection& operator() (const char* s)          { range r (s); return this->operator()(r); }
		Selection& operator() (const range& r);
		Selection& operator() (const Slice& s);
		Selection& operator() (const Any& a);

	private:
		Nodes_t nodes;
		const Node*  initial;
		
		void compact ();
		
		void select_using (predicate_t& p);
		void filter_using (predicate_t& p);
		static NilNode nil;
		
		friend class RuleSet;
		friend class RuleSetwDefault;
	};
	
	typedef std::unique_ptr<Selection> SelectionRef;

/*/// =========================================================================================================================================
	[CLAUSE]
/*/// -----------------------------------------------------------------------------------------------------------------------------------------
	struct RLtInt     { bool operator()(const range& r, long i) const   { return (r.asInteger()  < i); } };
	struct RLtEqInt   { bool operator()(const range& r, long i) const   { return (r.asInteger() <= i); } };
	struct RGtInt     { bool operator()(const range& r, long i) const   { return (r.asInteger() >  i); } };
	struct RGtEqInt   { bool operator()(const range& r, long i) const   { return (r.asInteger() >= i); } };
	struct REqInt     { bool operator()(const range& r, long i) const   { return (r.asInteger() == i); } };
	struct RNotEqInt  { bool operator()(const range& r, long i) const   { return (r.asInteger() != i); } };
	struct RLtReal    { bool operator()(const range& r, double i) const { return (r.asInteger()  < i); } };
	struct RLtEqReal  { bool operator()(const range& r, double i) const { return (r.asInteger() <= i); } };
	struct RGtReal    { bool operator()(const range& r, double i) const { return (r.asInteger() >  i); } };
	struct RGtEqReal  { bool operator()(const range& r, double i) const { return (r.asInteger() >= i); } };
	struct REqReal    { bool operator()(const range& r, double i) const { return (r.asInteger() == i); } };
	struct RNotEqReal { bool operator()(const range& r, double i) const { return (r.asInteger() != i); } };
	struct LtInt      { bool operator()(long i, long j) const           { return i  > j; } };
	struct LtEqInt    { bool operator()(long i, long j) const           { return i >= j; } };
	struct GtInt      { bool operator()(long i, long j) const           { return i <  j; } };
	struct GtEqInt    { bool operator()(long i, long j) const           { return i <= j; } };
	struct EqInt      { bool operator()(long i, long j) const           { return i == j; } };
	struct NotEqInt   { bool operator()(long i, long j) const           { return i != j; } };
	
	struct Quantify {
		range key;
		Quantify () { }
		Quantify (const range& akey) : key (akey) { }
	};
	
	template <typename L, typename R, typename O> struct RelExpr { };

	template <typename O> struct RelExpr <range, long, O> {
		range key;   long val;     O op;
		RelExpr (const range& akey, long aval) : key (akey), val (aval) { }
		
		bool operator() (const Node& n) const {
			if (n.isList() && n.children().size() >= 2 && n.children()[0]->getBytes() == key)
				return op (n.children()[1]->getBytes(), val);
			return false;
		}
	};

	template <typename O> struct RelExpr <range, double, O> {
		range key;   double val;     O op;
		RelExpr (const range& akey, double aval) : key (akey), val (aval) { }
		
		bool operator() (const Node& n) const {
			if (n.isList() && n.children().size() >= 2 && n.children()[0]->getBytes() == key)
				return op (n.children()[1]->getBytes(), val);
			return false;
		}
	};

	template <typename O> struct RelExpr <Quantify, long, O> {
		range key;   long val;     O op;
		RelExpr (const Quantify& quant, long aval) : key (quant.key), val (aval) { }

		bool operator() (const Node& n) const {
			Selection s (n);
			return op (s [key].size(), val);
		}
	};

	inline predicate_t operator<  (const range& k, long v) { return predicate_t (RelExpr <range, long, RLtInt>    (k, v)); }
	inline predicate_t operator<= (const range& k, long v) { return predicate_t (RelExpr <range, long, RLtEqInt>  (k, v)); }
	inline predicate_t operator>  (const range& k, long v) { return predicate_t (RelExpr <range, long, RGtInt>    (k, v)); }
	inline predicate_t operator>= (const range& k, long v) { return predicate_t (RelExpr <range, long, RGtEqInt>  (k, v)); }
	inline predicate_t operator== (const range& k, long v) { return predicate_t (RelExpr <range, long, REqInt>    (k, v)); }
	inline predicate_t operator!= (const range& k, long v) { return predicate_t (RelExpr <range, long, RNotEqInt> (k, v)); }

	inline predicate_t operator<  (const range& k, double v) { return predicate_t (RelExpr <range, double, RLtReal>    (k, v)); }
	inline predicate_t operator<= (const range& k, double v) { return predicate_t (RelExpr <range, double, RLtEqReal>  (k, v)); }
	inline predicate_t operator>  (const range& k, double v) { return predicate_t (RelExpr <range, double, RGtReal>    (k, v)); }
	inline predicate_t operator>= (const range& k, double v) { return predicate_t (RelExpr <range, double, RGtEqReal>  (k, v)); }
	inline predicate_t operator== (const range& k, double v) { return predicate_t (RelExpr <range, double, REqReal>    (k, v)); }
	inline predicate_t operator!= (const range& k, double v) { return predicate_t (RelExpr <range, double, RNotEqReal> (k, v)); }

	inline predicate_t operator<  (const Quantify& q, long v) { return predicate_t (RelExpr <Quantify, long, LtInt>    (q, v)); }
	inline predicate_t operator<= (const Quantify& q, long v) { return predicate_t (RelExpr <Quantify, long, LtEqInt>  (q, v)); }
	inline predicate_t operator>  (const Quantify& q, long v) { return predicate_t (RelExpr <Quantify, long, GtInt>    (q, v)); }
	inline predicate_t operator>= (const Quantify& q, long v) { return predicate_t (RelExpr <Quantify, long, GtEqInt>  (q, v)); }
	inline predicate_t operator== (const Quantify& q, long v) { return predicate_t (RelExpr <Quantify, long, EqInt>    (q, v)); }
	inline predicate_t operator!= (const Quantify& q, long v) { return predicate_t (RelExpr <Quantify, long, NotEqInt> (q, v)); }

	
/*/// =========================================================================================================================================
	[LEXER]
	The lexer splits the input into either '(' ')' or words. Space characters are used as delimiters unless escaped (or used in length
	prefixed sequences). The whole endeavor is made more complicated because we need to support symbols of unbounded length.
/*/// -----------------------------------------------------------------------------------------------------------------------------------------
	struct utf8reader {
		char32_t	code;
		std::string bytes;
		std::istream&    in;
		
		utf8reader (std::istream& aninput);
		~utf8reader ();
		
		inline bool   eof ()              { return in.eof (); }
		inline bool   fail ()             { return in.fail(); }
		inline size_t tell ()	          { return (size_t)in.tellg () - bytes.size(); }
		inline void   seek (size_t where) { in.seekg (where); }

		bool get ();     // cooked (we attempt to decode utf-8 here).
		bool getbyte (); // raw. only gets a byte.
	};

	enum class tokentype {
		unknown, lparen, rparen, qlparen, word, string
	};
	
	typedef bool (*acceptor_t)(utf8reader&);

	struct token {
		tokentype     type;
		range         bytes;
		size_t        line;
		
		token ();
		token (tokentype atype, const range& somebytes, size_t aline);
	};
	
	class Tokenizer {
	public:
		token       current;
		
		Tokenizer (utf8reader& asource, rangepool& apool);
		~Tokenizer ();
		
		bool advance ();
		
	private:
		inline void begin_accumulating () { used = 0; cpoint = src.tell (); }
		
		inline bool accumulate (std::string& s) {
			for (auto i : s) {
				if (used >= MaxWordSize) return false;
				accum[used] = i; ++used;
			}
			return true;
		}
		
		inline bool accumulator_has_bytes () { return (used > 0); }
	
		size_t find_next_delimiter (acceptor_t accept);
		void read_bytes_into (uint8_t* buf, size_t howmany);
		void read_chars_until (uint8_t* buf, size_t position, acceptor_t accept);
		range read_to_delimiter (acceptor_t accept);

		size_t get_number ();
		utf8reader& src;
		rangepool&  pool;
		size_t      line;

		static const size_t MaxWordSize = 8192;
		uint8_t     accum [MaxWordSize];
		unsigned	used;
		size_t      cpoint;
	};

/*/// =========================================================================================================================================
	[PARSER]
/*/// -----------------------------------------------------------------------------------------------------------------------------------------
	class Parser {
	public:
		Parser (Tokenizer& atokenizer);
		~Parser ();
		
		Node* get ();
	private:
		Tokenizer& tokens;
	};
	
/*/// =========================================================================================================================================
	[MATCHING]
	Here, we develop the rule machinery that allows us to make matches and generate output. XSLT uses a soup of rules, but we don't
	do that here. 
	
	Subclass pattern to create own patterns and actions.
	
	RuleSets use a global output pointer for writing. It defaults to being set to cout but it can be overridden.
/*/// -----------------------------------------------------------------------------------------------------------------------------------------

	class RuleSet;
	
	typedef std::function<bool(const Node&, RuleSet&)> rule_t;
	
	class RuleSet {
	public:
		RuleSet ();
		RuleSet (std::initializer_list<rule_t> l);
		virtual ~RuleSet ();
		
		virtual bool apply_to (const Node& n);
		virtual void apply_to_children (const Node& n);
		virtual void apply_to_selection (const Selection & s);
		virtual void apply_to_children (const Selection & s);
		virtual void apply_to_list (const Nodes_t & s);
		
		inline RuleSet& add (rule_t& r) { rules .push_back (r); return *this; }
		
		void apply_and_gather (const Node& n,      RuleSet& use, strings_t& into);
		void apply_and_gather (const Nodes_t& n,   RuleSet& use, strings_t& into);
		inline void apply_and_gather (const Selection& s, RuleSet& use, strings_t& into) { apply_and_gather (s.nodes, use, into); }
		
	protected:
		std::vector <rule_t> rules;
	};

	class RuleSetwDefault : public RuleSet {
	public:
		RuleSetwDefault ();
		RuleSetwDefault (std::initializer_list<rule_t> l);
		virtual ~RuleSetwDefault ();
		virtual bool apply_to (const Node& n);
		virtual void apply_to_selection (const Selection & s);
		virtual void apply_to_children (const Selection & s);
		virtual void apply_to_list (const Nodes_t & s);
	};

	// macros for building ruleset objects
#define def_rule_set(NAME)  se::RuleSet NAME = {
#define def_rule_set_w_default(NAME) se::RuleSetwDefault NAME = {
#define end_rule_set };

#define on_match(X) [](const se::Node& n, se::RuleSet& self) -> bool { se::Selection matched(n); if (matched X) {
#define perform(Y)  Y ; return true; } return false; }

#define when [](const se::Node& node, se::RuleSet& self) -> bool { se::Selection matched(node); if (matched
#define execute ) {
#define done ; return true; } return false; }

/*/// =========================================================================================================================================
	[STARTUP]
	Before using the framework, call the initialization procedure. When done, call the tear down procedure.
/*/// -----------------------------------------------------------------------------------------------------------------------------------------
	void initialize_sext ();
	void cleanup_sext ();

}

#endif