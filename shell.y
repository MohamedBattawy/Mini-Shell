
/*
 * CS-413 Spring 98
 * shell.y: parser for shell
 *
 * This parser compiles the following grammar:
 *
 *	cmd [arg]* [> filename]
 *
 * you must extend it to understand the complete shell grammar
 *
 */
%union	{
		char   *string_val;
	}
%token	<string_val> WORD

%token 	NOTOKEN GREAT NEWLINE RULER AMAZING BAD ERRORSIGN form1 form2 form3 form4 EXIT



%{
extern "C" 
{
	int yylex();
	void yyerror (char const *s);
	char * temp;
}
#define yylex yylex
#include <stdio.h>
#include "command.h"
%}

%%

goal:	
	commands
	;

commands: 
	command
	| commands command 
	;

command: simple_command
        ;

simple_command:
	EXIT{
		YYACCEPT;
	}
	|command_and_args iomodifier_opt NEWLINE {
		printf("   Yacc: Execute command\n");
		Command::_currentCommand.execute();
	}
	|command_and_args iomodifier_opt RULER commands{
		printf("   Yacc: Execute command\n");
		Command::_currentCommand.execute();
	}
	|command_and_args iomodifier_opt ERRORSIGN NEWLINE {
		printf("   Yacc: Execute command\n");
		Command::_currentCommand._background= 1;
		Command::_currentCommand.execute();
	}
	| NEWLINE 
	| error NEWLINE { yyerrok; }
	;

command_and_args:
	command_word arg_list {
		Command::_currentCommand.
			insertSimpleCommand( Command::_currentSimpleCommand );
	}
	;

arg_list:
	arg_list argument
	| /* can be empty */
	;

argument:
	WORD {
               printf("   Yacc: insert argument \"%s\"\n", $1);

	       Command::_currentSimpleCommand->insertArgument( $1 );
	}
	;

command_word:
	WORD {
               printf("   Yacc: insert command \"%s\"\n", $1);
	       Command::_currentSimpleCommand = new SimpleCommand();
	       Command::_currentSimpleCommand->insertArgument( $1 );
	}

	;

iomodifier_opt:
	GREAT WORD iomodifier_opt{
		printf("   Yacc: insert output \"%s\"\n", $2);
		Command::_currentCommand._outFile = $2;
	}
	| BAD WORD iomodifier_opt{
		printf("   Yacc: insert input \"%s\"\n", $2);
		Command::_currentCommand._inputFile = $2;
	}
	| AMAZING WORD iomodifier_opt{
		printf("   Yacc: insert output \"%s\"\n", $2);
		Command::_currentCommand._outFile = $2;
		Command::_currentCommand.write= 1;
	}
	| form1 WORD{
		printf("   Yacc: insert output \"%s\"\n", $2);
		Command::_currentCommand._errFile = $2;
		Command::_currentCommand.write= 0;
	}
	| form2 WORD{
		printf("   Yacc: insert output \"%s\"\n", $2);
		Command::_currentCommand._errFile = $2;
		Command::_currentCommand.write= 1;
	}
	| form3 WORD{
		printf("   Yacc: insert output \"%s\"\n", $2);
		Command::_currentCommand._errFile = $2;
		//Command::_currentCommand._outFile = $2;
		Command::_currentCommand.write= 0;
	}
	| form4 WORD{
		printf("   Yacc: insert output \"%s\"\n", $2);
		Command::_currentCommand._errFile = $2;
		Command::_currentCommand.write= 1;
	}
	| /* can be empty */ 
	;

%%

void
yyerror(const char * s)
{
	fprintf(stderr,"%s", s);
}

#if 0
main()
{
	yyparse();
}
#endif
