#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <stack>

#include <queue>
#include <string>
#include <iostream>
#include <typeinfo>



using namespace std;


/*** Enums and Print Functions for Terminals and Non-Terminals  **********************/

#define MAX_SYMBOL_NAME_SIZE 25

//all of the terminals in the language
typedef enum {
	T_eof = 0,	// 0: end of file
	T_num,		// 1: numbers
	T_plus,		// 2: +
	T_minus,	// 3: - 
	T_times,	// 4: *
	T_period,	// 5: .
	T_modulo, 	// 6: % 
	T_openparen,	// 7: (
	T_closeparen 	// 8: )
} token_type;

//this function returns a string for the token.  It is used in the parsetree_t
//class to actually dump the parsetree to a dot file (which can then be turned
//into a picture).  Note that the return char* is a reference to a local copy
//and it needs to be duplicated if you are a going require multiple instances
//simultaniously
char* token_to_string(token_type c) {
	static char buffer[MAX_SYMBOL_NAME_SIZE];
	switch( c ) {
		case T_eof: strncpy(buffer,"eof",MAX_SYMBOL_NAME_SIZE); break;
		case T_num: strncpy(buffer,"num",MAX_SYMBOL_NAME_SIZE); break;
		case T_plus: strncpy(buffer,"+",MAX_SYMBOL_NAME_SIZE); break;
		case T_minus: strncpy(buffer,"-",MAX_SYMBOL_NAME_SIZE); break;
		case T_times: strncpy(buffer,"*",MAX_SYMBOL_NAME_SIZE); break;
		case T_period: strncpy(buffer,".",MAX_SYMBOL_NAME_SIZE); break;
		case T_modulo: strncpy(buffer,"%",MAX_SYMBOL_NAME_SIZE); break;
		case T_openparen: strncpy(buffer,"(",MAX_SYMBOL_NAME_SIZE); break;
		case T_closeparen: strncpy(buffer,")",MAX_SYMBOL_NAME_SIZE); break;
		default: strncpy(buffer,"unknown_token",MAX_SYMBOL_NAME_SIZE); break;
	}
	return buffer;
}

//all of the non-terminals in the grammar (you need to add these in
//according to your grammar.. these are used for printing the thing out)
//please follow the convention we set up so that we can tell what the heck you
//are doing when we grade
typedef enum {
	epsilon = 100,
	NT_List,
	NT_Expr,
	NT_E, //Expr'
	NT_Term,
	NT_T, //Term'
	NT_Fact
} nonterm_type;

//this function returns a string for the non-terminals.  It is used in the parsetree_t
//class to actually dump the parsetree to a dot file (which can then be turned
//into a picture).  Note that the return char* is a reference to a local copy
//and it needs to be duplicated if you are a going require multiple instances
//simultaniously. 
char* nonterm_to_string(nonterm_type nt)
{
	static char buffer[MAX_SYMBOL_NAME_SIZE];
	switch( nt ) {
		  case epsilon: strncpy(buffer,"e",MAX_SYMBOL_NAME_SIZE); break;
		  case NT_List: strncpy(buffer,"List",MAX_SYMBOL_NAME_SIZE); break;
		  case NT_Expr: strncpy(buffer,"Expression",MAX_SYMBOL_NAME_SIZE); break;
		  case NT_E: strncpy(buffer,"E",MAX_SYMBOL_NAME_SIZE); break;
		  case NT_Term: strncpy(buffer,"Term",MAX_SYMBOL_NAME_SIZE); break;
		  case NT_T: strncpy(buffer,"T",MAX_SYMBOL_NAME_SIZE); break;
		  case NT_Fact: strncpy(buffer,"Fact",MAX_SYMBOL_NAME_SIZE); break;
		  default: strncpy(buffer,"unknown_nonterm",MAX_SYMBOL_NAME_SIZE); break;
		}
	return buffer;
}

/*** Scanner Class ***********************************************/

class scanner_t {
  public:

	//eats the next token and prints an error if it is not of type c
	void eat_token(token_type c);

	//peeks at the lookahead token
	token_type next_token();

	//return line number for errors
	int get_line();

	//constructor - inits g_next_token
	scanner_t();

	//Function scans and prints out every token
	//Only for use in debugging
	void debug_scanAll(void);

  private:

	//Keeps track of which line the scanner is on
	int line;

	//Holds next token
	token_type currentToken;
	bool needsNewToken;

	//This is a bogus member for implementing a useful stub, it should
	//be cut out once you get the scanner up and going.
	token_type bogo_token;

	//error message and exit if weird character
	void scan_error(char x);
	//error message and exit for mismatch
	void mismatch_error(token_type c);

};

token_type scanner_t::next_token()
{
	if(needsNewToken){
		//Read in input
		int ch = getchar();

		//Test Input to find Proper Token
		if(ch == '+') { currentToken = T_plus; }
		else if(ch == EOF) { currentToken = T_eof; }
		else if(ch == '-') { currentToken = T_minus; }
		else if(ch == '*') { currentToken = T_times; }
		else if(ch == '.') { currentToken = T_period; }
		else if(ch == '%') { currentToken = T_modulo; }
		else if(ch == '(') { currentToken = T_openparen; }
		else if(ch == ')') { currentToken = T_closeparen; }
		//if space, remove and recall the function
		else if(ch == '\b' || ch == '\t' || ch == 32){ next_token(); }
		//If newline, increment line number and recall function
		else if(ch == '\n'){ 
			line++;
			next_token();
		}
		//If it is a number we must scan the whole number
		else if(isdigit(ch)){
			//Hold string to just concatenate rather than shift
			string number = string();

			while(isdigit(cin.peek())){
				ch = getchar();
				number += ch;
			}
			//TODO find place to put number
			currentToken = T_num;
		}
		else{
			scan_error(ch);
		}

		needsNewToken = false;

	}

	//Return the currentToken
	return currentToken;
}

void scanner_t::eat_token(token_type c)
{
	//if we are supposed to eat token c, and it does not match
	//what we are supposed to be reading from file, then it is a 
	//mismatch error ( call - mismatch_error(c) )

	//Check type and return error
	//if types do not match.
	if(typeid(c) != typeid(currentToken)){
		mismatch_error(c);
	}

	//Tells future next tokens to continue reading input
	needsNewToken = true;
}

//Default Constructor
scanner_t::scanner_t()
{
	//Initialize member vars
	line = 1;
	needsNewToken = true;
}

//Returns the current line of the scanner
int scanner_t::get_line()
{
	return line;
}

//Returns a scanner error if the character present is unrecognized
void scanner_t::scan_error (char x)
{
	printf("scan error: unrecognized character '%c'\n",x);  
	exit(1);

}

//Returns an Error if the type expected does not match the type found
void scanner_t::mismatch_error (token_type x)
{
	printf("syntax error: found %s ",token_to_string(next_token()) );
	printf("expecting %s - line %d\n", token_to_string(x), get_line());
	exit(1);
}

void scanner_t::debug_scanAll( void ){
	next_token();
	while(currentToken != T_eof){
		cout << token_to_string(currentToken)<<" ";
		if(currentToken == T_period)
			cout << endl;
		eat_token(currentToken);
		next_token();
	}
}




/*** ParseTree Class **********************************************/

//just dumps out the tree as a dot file.  The interface is described below
//on the actual methods.  This class is full and complete.  You should not
//have to touch a thing if everything goes according to plan.  While you don't
//have to modify it, you will have to call it from your recursive decent
//parser, so read about the interface below.
class parsetree_t {
  public:
	void push(token_type t);
	void push(nonterm_type nt);
	void pop();
	void drawepsilon();
	parsetree_t();
  private:
	enum stype_t{
		TERMINAL=1,
		NONTERMINAL=0,
		UNDEF=-1
	};

	struct stuple { 
		nonterm_type nt;
		token_type t;
		stype_t stype; 
		int uniq;
	};
	void printedge(stuple temp); //prints edge from TOS->temp
	stack<stuple> stuple_stack;
	char* stuple_to_string(const stuple& s); 
	int counter;
};


//the constructer just starts by initializing a counter (used to uniquely
//name all the parse tree nodes) and by printing out the necessary dot commands
parsetree_t::parsetree_t()
{
	counter = 0;
	printf("digraph G { page=\"8.5,11\"; size=\"7.5, 10\"\n");
}

//This push function taken a non terminal and keeps it on the parsetree
//stack.  The stack keeps trace of where we are in the parse tree as
//we walk it in a depth first way.  You should call push when you start
//expanding a symbol, and call pop when you are done.  The parsetree_t
//will keep track of everything, and draw the parse tree as you go.
//This particular function should be called if you are pushing a non-terminal
void parsetree_t::push(nonterm_type nt)
{
	counter ++;
	stuple temp;
	temp.nt = nt;
	temp.stype = NONTERMINAL;
	temp.uniq = counter;
	printedge( temp );
	stuple_stack.push( temp );
}

//same as above, but for terminals
void parsetree_t::push(token_type t)
{
	counter ++;
	stuple temp;
	temp.t = t;
	temp.stype = TERMINAL;
	temp.uniq = counter;
	printedge( temp );
	stuple_stack.push( temp );
}

//when you are parsing a symbol, pop it.  That way the parsetree_t will
//know that you are now working on a higher part of the tree.
void parsetree_t::pop()
{
	if ( !stuple_stack.empty() ) {
		stuple_stack.pop();
	}

	if ( stuple_stack.empty() ) {
		printf( "}\n" );
	}
}

//draw an epsilon on the parse tree hanging off of the top of stack
void parsetree_t::drawepsilon()
{
	push(epsilon);
	pop();
}

// this private print function is called from push.  Basically it
// just prints out the command to draw an edge from the top of the stack (TOS)
// to the new symbol that was just pushed.  If it happens to be a terminal
// then it makes it a snazzy blue color so you can see your program on the leaves 
void parsetree_t::printedge(stuple temp)
{
	if ( temp.stype == TERMINAL ) {
		printf("\t\"%s%d\" [label=\"%s\",style=filled,fillcolor=powderblue]\n",
		  stuple_to_string(temp),
		  temp.uniq,
		  stuple_to_string(temp));
	} else {
		printf("\t\"%s%d\" [label=\"%s\"]\n",
		  stuple_to_string(temp),
		  temp.uniq,
		  stuple_to_string(temp));
	}

	//no edge to print if this is the first node
	if ( !stuple_stack.empty() ) {
		//print the edge
		printf( "\t\"%s%d\" ", stuple_to_string(stuple_stack.top()), stuple_stack.top().uniq ); 
		printf( "-> \"%s%d\"\n", stuple_to_string(temp), temp.uniq );
	}
}

//just a private utility for helping with the printing of the dot stuff
char* parsetree_t::stuple_to_string(const stuple& s) 
{
	static char buffer[MAX_SYMBOL_NAME_SIZE];
	if ( s.stype == TERMINAL ) {
		snprintf( buffer, MAX_SYMBOL_NAME_SIZE, "%s", token_to_string(s.t) );
	} else if ( s.stype == NONTERMINAL ) {
		snprintf( buffer, MAX_SYMBOL_NAME_SIZE, "%s", nonterm_to_string(s.nt) );
	} else {
		assert(0);
	}

	return buffer;
}


/*** Parser Class ***********************************************/

//the parser_t class handles everything.  It has and instance of scanner_t
//so it can peek at tokens and eat them up.  It also has access to the
//parsetree_t class so it can print out the parse tree as it figures it out.
//To make the recursive decent parser work, you will have to add some
//methods to this class.  The helper functions are described below

class parser_t {
  private:
	scanner_t scanner;
	parsetree_t parsetree;
	void eat_token(token_type t);
	void syntax_error(nonterm_type);

	//Recursive Descent Functions
	void List();
	void Expr();
	void E();
	void Term();
	void T();
	void Factor();

  public:	
	void parse();
};


//this function not only eats the token (moving the scanner forward one
//token), it also makes sure that token is drawn in the parse tree 
//properly by calling push and pop.
void parser_t::eat_token(token_type t)
{
	parsetree.push(t);
	scanner.eat_token(t);
	parsetree.pop();
}

//call this syntax error wehn you are trying to parse the
//non-terminal nt, but you fail to find a token that you need
//to make progress.  You should call this as soon as you can know
//there is a syntax_error. 
void parser_t::syntax_error(nonterm_type nt)
{
	printf("syntax error: found %s in parsing %s - line %d\n",
		token_to_string( scanner.next_token()),
		nonterm_to_string(nt),
		scanner.get_line() ); 
	exit(1); 
}


//One the recursive decent parser is set up, you simply call parse()
//to parse the entire input, all of which can be derived from the start
//symbol
void parser_t::parse()
{
	List();
}

//List is the Starting Non-terminal
void parser_t::List()
{
	//push this non-terminal onto the parse tree.
	//the parsetree class is just for drawing the finished
	//parse tree, and should in should have no effect the actual
	//parsing of the data
	parsetree.push(NT_List);

	Expr();
	eat_token(T_period);
	
	//Recursive call for list
	switch(scanner.next_token()){
	case T_eof: parsetree.drawepsilon(); break;
	default: List();
	}

	//now that we are done with List, we can pop it from the data
	//stucture that is tracking it for drawing the parse tree
	parsetree.pop();
}

//Only one rule to pick
void parser_t::Expr(){
	//Add Expr to parsetree
	parsetree.push(NT_Expr);

	Term();
	E();

	//Pop Expr from parsetree
	parsetree.pop();

}

//Expression Prime
void parser_t::E(){
	//Add E to parsetree
	parsetree.push(NT_E);

	switch(scanner.next_token()){
	case T_plus: 
		eat_token(T_plus); 
		Term();
		E();
		break;
	case T_minus: 
		eat_token(T_minus);
		Term();
		E();
		break;
	//Check FOLLOW Set for epsilon transitions
	//{ . , ) }
	case T_period: parsetree.drawepsilon(); break;
	case T_closeparen: parsetree.drawepsilon(); break;
	default:
		//Throw Error if plus or minus is not found
		syntax_error(NT_E);
	}

	//Pop E from parsetree
	parsetree.pop();
}

//Term has only one production
void parser_t::Term(){
	//Add Term to parsetree
	parsetree.push(NT_Term);

	Factor();
	T();

	//Pop Term from parsetree
	parsetree.pop();
}

void parser_t::T(){
	//Add T to parsetree
	parsetree.push(NT_T);

	switch(scanner.next_token()){
	case T_times:
		eat_token(T_times);
		Factor();
		T();
		break;
	case T_modulo:
		eat_token(T_modulo);
		Factor();
		T();
		break;
	//Check FOLLOW set for epsilon transisions
	//{ + - . ) }
	case T_plus: parsetree.drawepsilon(); break;
	case T_minus: parsetree.drawepsilon(); break;
	case T_period: parsetree.drawepsilon(); break;
	case T_closeparen: parsetree.drawepsilon(); break;
	default:
		//Throw error if no times or mod operators found
		syntax_error(NT_T);
	}

	//Pop T from parsetree
	parsetree.pop();
}

void parser_t::Factor(){
	//Add Factor to parsetree
	parsetree.push(NT_Fact);

	switch(scanner.next_token()){
	case T_num:
		eat_token(T_num);
		break;
	case T_openparen:
		eat_token(T_openparen);
		//Call Expression
		Expr();
		//Eat close paren
		eat_token(T_closeparen);
		break;
	default:
		//Throw error if no number or parenthesis
		syntax_error(NT_Fact);
	}

	//Pop Factor from parsetree
	parsetree.pop();
}

/*** Main ***********************************************/

int main()
{
	parser_t parser;
	parser.parse();
	return 0;
}
