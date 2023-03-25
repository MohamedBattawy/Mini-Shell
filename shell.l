
/*
 *
 * CS-413 Spring 98
 * shell.l: lexical analyzer for shell
 * You have to extend it.
 *
 */

%{

#include <string.h>
#include "y.tab.h" //for the yacc to understand GREAT =50 as yylex() returns integers which will be 50 (from y.tab.h can be any value)

%}

%%

exit {
	return EXIT;
}
\n 	{
		return NEWLINE;
	}

[ \t]	{
		/* Discard spaces and tabs */
     	}
>&		{
		return form1;
		}
>>&		{
		return form2;
		}
&>		{
		return form3;
		}
&>>		{
		return form4;
		}
>>		{
		return AMAZING;
		}
"<"		{
		return BAD;
		}

"|"		{
		return RULER;
		}

"&"		{
		return ERRORSIGN;
		}

">" 	{
		return GREAT;
		}

[^ \t\n\>\<\|\&]+  {
		/* Assume that file names have only alpha chars */
		yylval.string_val = strdup(yytext);
		return WORD;
	}

.	{
		/* Invalid character in input */
		return NOTOKEN;
	}

%%
	
