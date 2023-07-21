#include <iostream>
#include <vector>
#include <stack>
#include <sstream>
#include <stdint.h>
#include <stdlib.h>
#include <ctype.h>

using namespace std;

enum class Tokens {

	NUMBER,
	PLUS,
	MINUS,
	MUL,
	DIV,
	OPBRACE,
	CLBRACE
};

struct Token {

	string value;
	Tokens type;
	Token ( string v, Tokens t ) : value ( v ), type ( t ) {}
};

/* Turn a string into group of tokens */
vector<Token> Tokenize ( string& expression ) {

	vector<Token> tokens;
	string num;
	for ( uint64_t i = 0; i < expression.size (); ++i ) {

		if ( expression [i] == '+' ) tokens.push_back ( Token ( "+", Tokens::PLUS ) );
		else if ( expression [i] == '-' ) tokens.push_back ( Token ( "-", Tokens::MINUS ) );
		else if ( expression [i] == '*' ) tokens.push_back ( Token ( "*", Tokens::MUL ) );
		else if ( expression [i] == '/' ) tokens.push_back ( Token ( "/", Tokens::DIV ) );
		else if ( expression [i] == '(' ) tokens.push_back ( Token ( "(", Tokens::OPBRACE ) );
		else if ( expression [i] == ')' ) tokens.push_back ( Token ( ")", Tokens::CLBRACE ) );
		else if ( isblank ( expression [i] ) ) continue;
		else if ( isdigit ( expression [i] ) ) {

			while ( isdigit ( expression [i] ) ) {

				num += expression [i];
				++i;
			}

			tokens.push_back ( Token ( num, Tokens::NUMBER ) );
			num.clear ();
			--i;
		} else {

			cerr << "Error: Unexpected Token '" << expression [i] << "'" << endl;
		}
	}

	return tokens;
}

uint8_t precedence ( Tokens t ) {

	switch ( t ) {

		case Tokens::DIV:
		case Tokens::MUL:
			return 2;
		case Tokens::PLUS:
		case Tokens::MINUS:
			return 1;
		default:
			return 0;
	}
}


/* postfix Utils */
uint64_t toNum ( string num ) {

	stringstream s ( num );

	int64_t n;
	s >> n;
	return n;
}

bool isOperator ( Tokens t ) {

	if ( t == Tokens::NUMBER ) return false;

	return true;
}

bool plusMinus ( Tokens t ) {

	if ( t == Tokens::PLUS || t == Tokens::MINUS ) return true;
	return false;
}

// this function will take care about unarry Operator
void util ( vector<Token>& tokens ) {

	for ( uint64_t i = 0; i < tokens.size (); ++i ) {

		if ( i == 0 && plusMinus ( tokens [i].type ) ) {

			tokens [i + 1].value = (( tokens [i].type == Tokens::MINUS )? "-": "+") + tokens [i + 1].value;
			tokens.erase ( tokens.cbegin () + i );
		} else if ( plusMinus ( tokens [i].type ) && isOperator ( tokens [ i - 1 ].type ) ) {

			tokens [i + 1].value = (( tokens [i].type == Tokens::MINUS )? "-": "+" )+ tokens [i + 1].value;
			tokens.erase ( tokens.cbegin () + i );
		} else continue;
	}
}

// turn the tokens list into a postFix expression
vector<Token> postFixEXP ( vector<Token>& tokens ) {

	vector<Token> toks;
	stack<Token> helper; // helper stack!

	util ( tokens );
	for ( auto token : tokens ) {

		if ( !isOperator ( token.type ) ) {

			toks.push_back ( token );
		} else if ( token.type == Tokens::OPBRACE ) {

			helper.push ( token );
		} else if ( token.type == Tokens::CLBRACE ) {

			while ( helper.top ().type != Tokens::OPBRACE ) {

				if ( helper.empty () ) {

					cerr << "Invalid expression" << endl;
					abort ();
				}

				toks.push_back ( helper.top () );
				helper.pop ();

			}

			helper.pop ();
		} else {

			while ( !helper.empty () && precedence ( helper.top ().type ) >= precedence ( token.type ) ) {


				toks.push_back ( helper.top () );
				helper.pop ();
			}

			helper.push ( token );
		}
	}

	while ( !helper.empty () ) {

		toks.push_back ( helper.top () );
		helper.pop ();
	}

	return toks;
}

struct TreeNode {

	TreeNode * left, * right;
	Token token;
	TreeNode ( Token t ) : left ( nullptr ), right ( nullptr ), token ( t ) {}

};

void solve ( TreeNode * root ) {

	if ( root->left ) {

		if ( root->left->token.type != Tokens::NUMBER ) solve ( root->left );
	}
	if ( root->right ) {


		if ( root->right->token.type != Tokens::NUMBER ) solve ( root->right );
	}

	if ( !root->left && !root->right ) return;

	Tokens t = root->token.type;
	int64_t res = toNum ( root->right->token.value );
	int64_t tmp;

	switch ( t ) {

		case Tokens::PLUS:
			res += toNum ( root->left->token.value );
			break;
		case Tokens::MINUS:
			res -= toNum ( root->left->token.value );
			break;
		case Tokens::MUL:
			res *= toNum ( root->left->token.value );
			break;
		case Tokens::DIV:
			tmp = toNum ( root->left->token.value );
			if ( !tmp ) {

				cerr << "Divide by zero error!"<<endl;
				abort ();
			}

			res /= tmp;
			break;
	} 

	root->token.value = to_string ( res );

	delete root->left, root->right;
}

/* Evaluate the post-fix expression */
int64_t Evaluate ( vector<Token>& expression ) {

	stack<TreeNode *> helper; // another helper stack

	for ( auto k : expression ) {

		if ( isOperator ( k.type ) ) {

			TreeNode * t = new TreeNode ( k );
			TreeNode * t1, * t2;
			if ( helper.empty () ) {

				cerr << "Invalid expression" << endl;
				abort ();
			}

			t1 = helper.top ();
			helper.pop ();

			if ( helper.empty () ) {

				cerr << "Invalid expression" << endl;
			}

			t2 = helper.top ();

			helper.pop ();

			t->left = t1;
			t->right = t2;

			helper.push ( t );
		} else {

			TreeNode * node = new TreeNode ( k );

			helper.push ( node );
		}
	}


	TreeNode * root = helper.top ();

	solve ( root );

	return toNum ( root->token.value );
}

int main ( int argc, char ** argv ) {


	if ( argc < 2 ) {

		cerr << "Error: usage " << argv [0] << " <Expression>" << std::endl;
		return EXIT_FAILURE;
	}
	string exp = argv [1]; // the expression

	auto a = Tokenize ( exp );

	auto b = postFixEXP ( a );

	cout << "result is: " << Evaluate ( b ) << endl;
	return EXIT_SUCCESS;
}
