//
//  validate.h
//  validateschema
//
//  Created by Theo Johnson on 10/2/13.
//  Copyright (c) 2013 Theo Johnson. All rights reserved.
//

#ifndef __validateschema__validate__
#define __validateschema__validate__

#include <iostream>
#include "sext.h"

/*/// =========================================================================================================================================
	Validator for S-Expression Schema Language
	
	Syntax:
	(schema
		(element schema     (has (many members)))
		(class members      '(element enum class type import))
		(element element    (has (& identifier has)))
		(element has        (has (list element-expr))
		(element is         (has element-expr))
		
		(class element-expr '(identifier many some maybe & | > < = (list element-expr)))
		(element many		(has element-expr))
		(element some		(has element-expr))
		(element maybe      (has element-expr))
		(element &          (has (many element-expr)))
		(element |          (has (many element-expr)))
		(element >          (has (& number element-expr)))
		(element <          (has (& number element-expr)))
		(element =          (has (& number element-expr)))
	
		(element type       (has (& identifier has)))
		(element class      (has (& identifier (list identifier))))
		(element enum       (has (& identifier (list identifier))))
		(type identifier    (is text))
		
		(element import     (has text))
	
	Meanings:
	schema			- The root of the tree of our s-expression. Indicator to a validator that this is an S-expression schema
	
	element			- Like XML, defines an element and the other things that may occur within.
					  Within a conforming document, elements are the head of a list ie:  (html ... )
	
	class           - Specifies that any of the specified items may occur where the class is required -- or relationship.
					  Classes are not explicitly represented ina conforming doc.
					  
	enum			- Similar to class, but the values represent explicit text that must occur
					  ie:  (enum genre-name (sci-fi comedy romance)) would require that only one of the strings
					  "sci-fi", "comedy" or "romance" must occur.
					  
	type			- Declares that an identifier is an alias for a pattern. Similar to typedefs in C.
	
	import			- Imports an external schema. The external schema is merged into the caller. Names defined in the
					  caller override those in the import.
					  
	has				- Specifies the elements that appear as part of an element. The elements may appear in any
					  order unless an in-order contraint is used.

	is				- Syntax used with type to define a type alias.
	
	many			- Constraint on an object to indicate that 0 or more of that type of object may occur.
	some			- Constraint on an object to indicate that 1 or more of that type of object may occur.
	maybe			- Constraint on an object to indicate that its occurrence is optional
	&         		- Constraint on a list of objects to indicate they must occur in the order listed
	|         		- Constraint on a list of objects to indicate they may occur in any order. This is the default interpretation.
					  It only needs to be explicitly indicated when a surrounding specification has been
					  qualified with &.
	>        		- Constraint on an object to indicate that it must occur subject to a minimum quantity.
	<               - Constraint on an object to indicate that it must occur subject to a maximum quantity.
	=        		- Constraint on an object to indicate that it must occur subject to an exact quantity.
	
	list			- Indicates that the object must be wrapped in a list, ie: (a b c).
	
/*/// -----------------------------------------------------------------------------------------------------------------------------------------

class SchemaValidator {
public:
	SchemaValidator (std::ostream& alog);
	~SchemaValidator ();
	
	bool isValid (const se::Node& t);
	
private:
	std::ostream& log;

	bool matchText (const se::Node& t);
	bool matchNumber (const se::Node& t);
	bool matchListText (const se::Node& t);
	bool matchListElementExpr (const se::Node& t);
	bool matchSomeElementExpr (const se::Nodes_t& items);
	bool matchElementExpr (const se::Node& t);
	bool matchMany (const se::Node& t);
	bool matchSome (const se::Node& t);
	bool matchMaybe (const se::Node& t);
	bool matchLtQuantifier (const se::Node& t);
	bool matchGtQuantifier (const se::Node& t);
	bool matchEqQuantifier (const se::Node& t);
	bool matchAnd (const se::Node& t);
	bool matchOr (const se::Node& t);
	bool matchHas (const se::Node& t);
	bool matchIs (const se::Node& t);
	bool matchElement (const se::Node& t);
	bool matchEnum (const se::Node& t);
	bool matchClass (const se::Node& t);
	bool matchType (const se::Node& t);
	bool matchImport (const se::Node& t);
	bool matchMember (const se::Node& t);
	bool matchSomeMembers (const se::Nodes_t& items);
	bool matchSchema (const se::Node& t);
};


#endif /* defined(__validateschema__validate__) */
