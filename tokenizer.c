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

typedef enum { START ,ZERO ,  OCTAL , HEX , DIGIT , FLOAT , PLUSMIN ,DOT , EXP, INDEF } STATE ;

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
int isExp(char *a);
int isPlusMinus(char *a);
STATE stateTest(char *p , TokenizerT * tk);
STATE stateAndCharTest(char *p , TokenizerT * tk);
STATE charTest(char *p);
void stateTokenPrint(char * p , TokenizerT *tk );



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
	//free (tk);
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

	
	if(*p == '\0')
		return NULL;

	int tokenBeg = tk->_processedLen ;  // token begins from this point onwards. 
	
	
	while(*p != '\0')
	{
		if(isSpace(p))
		{
			int tokenLen =	tk->_processedLen - tokenBeg ;  
			char * token = (char*)malloc(sizeof(char)* (tokenLen + 1) ) ;
			__strncpy(token , ( tk->_str  + tokenBeg) , tokenLen);	

			++tk->_processedLen; // we disregard this character during the next try
			
			printf("SPACE CHAR");

			return token;		
		} 

		// STUPID IMPLEMENTATION 
		STATE prevState = tk->_state;

		printf("%d",tk->_state);
		tk->_state = stateAndCharTest(p,tk);		

	printf("%d",tk->_state);
	printf("%c",*p);
	printf("%c\n",*(p+1));
	
		if(tk->_state != prevState) // if transisition between two states then we know 
		{
			
			printf("PREV : %d ",prevState);
			printf("CURRENT : %d\n",tk->_state);
			if(prevState != START &&prevState != ZERO) // allow transition from start and zero
			{
				if(prevState != DOT && prevState != EXP && prevState!= PLUSMIN) // allow transition from dot and exp and plusMinus
				{
					if(tk->_state != EXP && tk->_state != DOT && tk->_state != PLUSMIN) // allow transition to exp and dot and plus minus
					{
					//	printf("STATE CHANGE : TOKEN FORMED\n");
						break ; 
					}
				}
			}
		}
		
		++tk->_processedLen;
		++p;
	}
	
	int tokenLen =	tk->_processedLen - tokenBeg ;  
	char * token = (char*)malloc(sizeof(char)* (tokenLen + 1) ) ;
	// COPY FROM STRING TO TOKEN 
	__strncpy(token , ( tk->_str  + tokenBeg) , tokenLen);	

	return token;
}



STATE stateAndCharTest(char *p , TokenizerT * tk)
{
	STATE st ;
	switch(tk->_state)
	{
		case ZERO:
		       	if(isX(p))
				st = HEX;
			else if(isDot(p))
				st = DOT;
			else if(isOctal(p))
				st = OCTAL;
			else 
				st = INDEF;
			break; 

		case OCTAL:
			if(isOctal(p))
				st = OCTAL;
			else
				st = INDEF;
			break; 

		case HEX:
			if(isHex(p))
				st = HEX;				
			else
				st = INDEF;
			break;

		case DOT:
			if(isDigit(p))
				st = FLOAT;
			else if(isExp(p))
				st = EXP;
			else
				st = INDEF;
			break;

		case DIGIT:
			if(isDigit(p))	
				st = DIGIT; 
			else if(isDot(p))
				st = DOT;
			else if(isExp(p))
				st = EXP;
			else
				st = INDEF;
			break;

		case EXP:
			if(isPlusMinus(p))
				st = PLUSMIN;
			else if(isDigit(p))
				st = FLOAT;
			else 
				st = INDEF;
			break;
	
		case FLOAT: 
			if(isDigit(p))
				st = FLOAT;
			else if(isExp(p))
				st = EXP;
			else
				st = INDEF;
			break; 

		case PLUSMIN:
			if(isDigit(p))
				st = FLOAT;
			else
				st = INDEF;
			break;

		case START:
			if ( *p == '0')
				st = ZERO;
			else if(isDigit(p))
			{
				printf("DIGIT");
				st = DIGIT;
			}
			else
				st = INDEF;
			break;
		case INDEF:
		default:
			st = INDEF; // ?? 
				break ; 
	}
	return st;
}

/*
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
	else if(isExp(p))
	{
		st = EXP;
	}
	else if(isPlusMinus(p))
	{
		st = PLUSMIN;
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

*/

/*
 * param p : pointer to c string
 * param tk: pointer to tokenizer
 *
 * FUNC : will print state and token to stdout
 * SIDE-EFFECT : none
 *
 */
void stateTokenPrint(char * p , TokenizerT *tk )
{
	printf("STATE : %d",tk->_state);
	if(tk->_state == DIGIT)
		printf("decimal \"%s\"\n",p);
	else if(tk->_state == OCTAL)
		printf("octal \"%s\"\n",p);
	else if(tk->_state == FLOAT)
		printf("float \"%s\"\n",p);
	else if(tk->_state == HEX)
		printf("hex \"%s\"\n",p);
	else if(tk->_state == ZERO)
		printf("zero \"%s\"\n",p);
	else if(tk->_state == INDEF)
		printf("mal \"%s\"\n",p);
	else 
		printf("mal \"%s\"\n",p);

	// resetting the token for the next traversal 
		tk->_state = START; // START OVER AGAIN
}

int isOctal(char *a)
{	
	int b = (int)(*a - '0');
	return b>= 0 && b <= 7 ; 
}

int isDigit(char *a)
{
	return isdigit(*a);
}

int isX(char *a)
{
	return (*a == 'x' || *a == 'X');
}

int isHex(char *a)
{
	return isxdigit(*a);
}

int isSpace(char *a)
{
	return isspace(*a);
}

int isDot(char *a)
{
	return (*a=='.');
}

int isPlusMinus(char *a)
{
	return (*a=='+' || *a=='-');
}

int isExp(char *a)
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
		if(res == NULL)
			break ; 
		stateTokenPrint(res,tk);
	}	

	TKDestroy(tk);

//	char * id = argv[1]; 
//	for(; *id!= 0 ; ++id)
//		printf(" > %c",*id);

//	char x = 'X';
//	printf("X : %d ", isX(&x));

  return 0;
}
