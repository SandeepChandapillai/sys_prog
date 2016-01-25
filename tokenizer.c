/*
 * tokenizer.c
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

int __strncpy(char * dest, char * src , int len)
{
	int i ;
	for(i = 0 ; i < len ; ++i)
	{
		*(dest + i) = *(src + i);
	}
	dest[len] = '\0';	
	return 0 ; 
}


/*
 * Tokenizer type.  You need to fill in the type as part of your implementation.
 */

typedef enum { START ,ZERO ,  OCTAL , HEX , DIGIT , FLOAT , INT ,ESCAPE ,DOT , EXP , INDEF } STATE ;

typedef struct TokenizerT_ TokenizerT;
struct TokenizerT_ {
	char * _str; // Points to allocated string copy OR better to use array ? 
	int _strLen; // size
	int _processedLen ; // length of string processed
	STATE _state ; 
};



/*
 *
 * Function Prototypes
 *
 *
 */
int isOctal(char *a);
int isDigit(char *a);
int isX(char *a);
int isHex(char *a);
int isSpace(char *a);
int isDot(char *a);
int isEXP(char *a);
STATE stateTest(char *p , TokenizerT * tk);
STATE charTest(char *p);




/*
 * TKCreate creates a new TokenizerT object for a given token stream
 * (given as a string).
 * 
 * TKCreate should copy the arguments so that it is not dependent on
 * them staying immutable after returning.  (In the future, this may change
 * to increase efficiency.)
 *
 * If the function succeeds, it returns a non-NULL TokenizerT.
 * Else it returns NULL.
 *
 * You need to fill in this function as part of your implementation.
 */

TokenizerT *TKCreate( char * ts ) {
	TokenizerT *res  = (TokenizerT *)malloc(sizeof(TokenizerT)); 
	if( res == NULL)
		return NULL ; 
	res->_strLen = strlen(ts);
	
	// Allocate string space 	
	res->_str = (char *)malloc(sizeof(char)*res->_strLen);
	strcpy(res->_str , ts);
	res->_processedLen = 0 ; 
	res->_state = START;
  return res;
}

/*
 * TKDestroy destroys a TokenizerT object.  It should free all dynamically
 * allocated memory that is part of the object being destroyed.
 *
 * You need to fill in this function as part of your implementation.
 */

void TKDestroy( TokenizerT * tk ) {
	free (tk->_str);
	free (tk);
}
/*
 * TKGetNextToken returns the next token from the token stream as a
 * character string.  Space for the returned token should be dynamically
 * allocated.  The caller is responsible for freeing the space once it is
 * no longer needed.
 *
 * If the function succeeds, it returns a C string (delimited by '\0')
 * containing the token.  Else it returns 0.
 *
 * You need to fill in this function as part of your implementation.
 */

char *TKGetNextToken( TokenizerT * tk ) {




/* LOOP FORWARD UNTIL WE SEE : 
 *  1) WHITE SPACE 
 *  2) /0 
 *  3) CHANGE FROM ONE STATE TO ANOTHER STATE. 
 */

char * p = tk->_str + tk->_processedLen ; // points to the last processed item 
int tokenBeg = tk->_processedLen ;  // token begins from this point onwards. 

	while(*p != '\0')
	{
	
	// STUPID IMPLEMENTATION 
	STATE prevState = tk->_state;
	if(tk->_state == START) // START STATE
	{
		tk->_state = stateTest(p,tk);
	}	
	else // CONTINOUS STATE TRANSITION
	{
		tk->_state = stateTest(p,tk);
	}

	printf("%d",tk->_state);
	printf("%c\n",*p);




		if(tk->_state != prevState && prevState != START)
			break ; 

		++tk->_processedLen;
		++p;
	}
	
	int tokenLen =	tk->_processedLen - tokenBeg + 1;  
	printf("TOKEN LEN %d " , tokenLen);
	char * token = (char*)malloc(sizeof(char)* (tokenLen + 1) ) ;
	// COPY FROM STRING TO TOKEN 
	__strncpy(token , ( tk->_str  + tokenBeg) , tokenLen);	

	printf("12 :: %s", token);	
	return token;
}



STATE stateAndCharTest(char *p , TokenizerT * tk)
{
	
	STATE st ;
	switch(tk->_state)
	{
		case HEX:
			if(isHex(p))
				st = HEX;				
			else
				st = charTest(p); 			
			break;
		case DIGIT:
			if(isDigit(p))	
				st = DIGIT; 
			else if(isSpace(p))
				st = INT; // ABSORPTION STATE
			else if(isDot(p))
				st = DOT;
			else
				st = charTest(p);
			break;

		case OCTAL:
			if(isOctal(p))
				st = OCTAL;
			else
				st = charTest(p);
			break; 
		case FLOAT: 
			if(isDigit(p))
				st = FLOAT;
			else
				st = charTest(p);
			break; 
		case ZERO:
			st = charTest(p);
			if(st == DOT)
				st = FLOAT;
			else if(st == HEX)
				st = HEX;
			else if(st == DIGIT || st == OCTAL)
				st = OCTAL;
			break; 
		case DOT:
			st = charTest(p);
			if(st == DIGIT)
				st = FLOAT;
			else
				st = INDEF;
		case EXP:

		default:
			st = charTest(p);
	}
	return st;
}


STATE charTest(char *p)
{
	STATE st ;
	if(*p == '0')
	{
		st = ZERO;
	}
	else if(isSpace(p))
	{
		st = INDEF;
	}
	else if(isDot(p))
	{
		st = DOT;
	}
	else if( isX(p))
	{
			st = HEX;
	}
	else if(isEXP(p))
	{
		st = EXP;
	}
	else if(isDigit(p))
	{
		st = DIGIT;
	}
	else if( isHex(p))
	{
			st = HEX;
	}
	else if( isOctal(p))
	{
		st = OCTAL;
	}
	else
	{
		st = INDEF;
	}	
	return st;
}



int isOctal(char *a)
{
	return atoi(a) <= 7 ; 
}

int isDigit(char *a)
{
	return isdigit(atoi(a)); 
}

int isX(char *a)
{
	return (*a == 'x' || *a == 'X');
}

int isHex(char *a)
{
	return isxdigit(atoi(a));
}

int isSpace(char *a)
{
	return isspace(atoi(a));
}

int isDot(char *a)
{
	return (*a=='.');
}

int isPlusMinus(char *a)
{
	return (*a=='+' || *a=='-');
}

int isEXP(char *a)
{
	return (*a=='e' || *a=='E');
}

/*
 * main will have a string argument (in argv[1]).
 * The string argument contains the tokens.
 * Print out the tokens in the second string in left-to-right order.
 * Each token should be printed on a separate line.
 */

int main(int argc, char **argv) {

	/* check the input */ 
	if(argc < 2)
	{
		printf("Usage %s <string>\n ",argv[0]);
		return -1 ; 
	}	

	TokenizerT * tk = TKCreate(argv[1]);
	printf(" STRING : %s\n", tk->_str);

	while(1)
	{
		char * res = TKGetNextToken(tk);	
		printf(	" : %s\n",res);		
		break;
	}	



	TKDestroy(tk);
	



  return 0;
}
