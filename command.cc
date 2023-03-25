
/*
 * CS354: Shell project
 *
 * Template file.
 * You will need to add more code here to execute the command table.
 *
 * NOTE: You are responsible for fixing any bugs this code may have!
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include <wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include "command.h"
#include <fstream>
#include <iostream>
using namespace std;
SimpleCommand::SimpleCommand()
{
	// Creat available space for 5 arguments
	_numberOfAvailableArguments = 5;
	_numberOfArguments = 0;
	_arguments = (char **) malloc( _numberOfAvailableArguments * sizeof( char * ) );
}

void
SimpleCommand::insertArgument( char * argument )
{
	if ( _numberOfAvailableArguments == _numberOfArguments  + 1 ) {
		// Double the available space
		_numberOfAvailableArguments *= 2;
		_arguments = (char **) realloc( _arguments,
				  _numberOfAvailableArguments * sizeof( char * ) );
	}
	
	_arguments[ _numberOfArguments ] = argument;

	// Add NULL argument at the end
	_arguments[ _numberOfArguments + 1] = NULL;
	
	_numberOfArguments++;
}

Command::Command()
{
	// Create available space for one simple command
	_numberOfAvailableSimpleCommands = 1;
	_simpleCommands = (SimpleCommand **)
		malloc( _numberOfSimpleCommands * sizeof( SimpleCommand * ) );

	_numberOfSimpleCommands = 0;
	_outFile = 0;
	_inputFile = 0;
	_errFile = 0;
	_background = 0;
}

void
Command::insertSimpleCommand( SimpleCommand * simpleCommand )
{
	if ( _numberOfAvailableSimpleCommands == _numberOfSimpleCommands ) {
		_numberOfAvailableSimpleCommands *= 2;
		_simpleCommands = (SimpleCommand **) realloc( _simpleCommands,
			 _numberOfAvailableSimpleCommands * sizeof( SimpleCommand * ) );
	}
	
	_simpleCommands[ _numberOfSimpleCommands ] = simpleCommand;
	_numberOfSimpleCommands++;
}

void
Command:: clear()
{
	for ( int i = 0; i < _numberOfSimpleCommands; i++ ) {
		for ( int j = 0; j < _simpleCommands[ i ]->_numberOfArguments; j ++ ) {
			free ( _simpleCommands[ i ]->_arguments[ j ] );
		}
		
		free ( _simpleCommands[ i ]->_arguments );
		free ( _simpleCommands[ i ] );
	}

	if ( _outFile ) {
		free( _outFile );
	}

	if ( _inputFile ) {
		free( _inputFile );
	}

	if ( _errFile ) {
		free( _errFile );
	}

	_numberOfSimpleCommands = 0;
	_outFile = 0;
	_inputFile = 0;
	_errFile = 0;
	_background = 0;
}

void
Command::print()
{
	printf("\n\n");
	printf("              COMMAND TABLE                \n");
	printf("\n");
	printf("  #   Simple Commands\n");
	printf("  --- ----------------------------------------------------------\n");
	
	for ( int i = 0; i < _numberOfSimpleCommands; i++ ) {
		printf("  %-3d ", i+1 );
		for ( int j = 0; j < _simpleCommands[i]->_numberOfArguments; j++ ) {
			printf("\"%s\" \t", _simpleCommands[i]->_arguments[ j ] );

			//ls aaa | ly xxx yyy        ls aaa is simple command 1 and ly xxx yyy is simple command 2
			//ls is argument[0] of simple command 1, aaa is argument[1] of simple command 1
			//ys is argument[0] of simple command 2, xxx is argument[1] of simple command 2 and yyy is argument[2] of simple command 2
		}
		printf("\n");
	}

	printf( "\n\n" );
	printf( "  Output       Input        Error        Background   WriteMode\n" );
	printf( "  ------------ ------------ ------------ ------------ ---------\n" );
	printf( "  %-12s %-12s %-12s %-12s %-12s\n", _outFile?_outFile:"default", // ? is the ternary operator, if outFile doesn't exist (NULL or 0) print default
		_inputFile?_inputFile:"default", _errFile?_errFile:"default",
		_background?"YES":"NO",write?"Append":"Default");
	printf( "\n\n" );
	
}

FILE *lgfile;

void zombie_handler(int sig)
{

	int status,x;
	x= wait(&status);
	time_t now = time(0);
	if(x>=0){	
	fprintf(lgfile,"child with id %d was terminated with status %d on %s\n",x,status,ctime(&now));
	}}


void
Command::execute()
{
	// Don't do anything if there are no simple commands
	if ( _numberOfSimpleCommands == 0 ) {
		prompt();
		return;
	}
	if(_simpleCommands[0]->_arguments[0] == "cd"){
		printf("found command cd\n");
		if(_simpleCommands[0]->_numberOfArguments > 1){
			perror("too many arguments\n");
		}
		else if(_simpleCommands[0]->_arguments[1]){
			chdir(_simpleCommands[0]->_arguments[1]);
			printf("sucess\n");
		}
		else{
			chdir(getenv("HOME"));
			printf("directory is home now");
		}
	}
	// Print contents of Command data structure
	print();

	printf("command is %s\n",_simpleCommands[0]->_arguments[0]);
	

	pid_t pid;
	int status,defaultin=0,defaultout=0,defaulterr=0,outdis=0,indis=0,errdis=0;
	int fdpipe [_numberOfSimpleCommands-1][2];


	defaultin = dup(0);
	defaultout = dup(1);
	defaulterr = dup(2);

	if(_errFile){
		printf("errfile changing\n");
		if(!write)
		errdis = creat(_errFile,0777);
		else
		errdis = open(_errFile,O_WRONLY|O_APPEND|O_CREAT,0777);
		dup2(errdis,2);
		dup2(errdis,1);
	}
	if(_outFile){
		printf("output changing\n");
		if(!write)
		outdis = creat(_outFile,0777);
		else{
		outdis = open(_outFile,O_WRONLY|O_APPEND|O_CREAT,0777);
		}
		dup2(outdis,1);
	}
	if(_inputFile){
		indis = open(_inputFile,O_RDONLY);
		dup2(indis,0);
	}
	
	for(int i=0 ; i<_numberOfSimpleCommands;i++){
		if(!strcmp(_simpleCommands[0]->_arguments[0],"cd")){
		printf("found command cd\n");
		if(_simpleCommands[0]->_numberOfArguments > 2){
			perror("too many arguments\n");
		}
		else if(_simpleCommands[0]->_arguments[1]){
			//chdir(_simpleCommands[0]->_arguments[1]);
			printf(chdir(_simpleCommands[0]->_arguments[1])==0?"sucess\n":"fail\n");
			continue;
		}
		else{
			//chdir(getenv("HOME"));
			//printf("\n%s\n",getenv("HOME"));
			printf(chdir(getenv("HOME"))==0?"sucess\n":"fail\n");
			continue;
		}
	}
			if(_numberOfSimpleCommands>1){
			pipe(fdpipe[i]);
			if(i==0){
				printf("set output to pipe i=%d\n",i);
				dup2(fdpipe[0][1],1);
				/*printf("this should not be printed");*/
				}
			if(i>=1 && i < _numberOfSimpleCommands-1){
				printf("set output and input to pipe i=%d\n",i);
				dup2(fdpipe[i-1][0],0);
				dup2(fdpipe[i][1],1);
				//if (indis){ close(indis); indis=0;}
			}
			if(i==_numberOfSimpleCommands-1){
				printf("set input to pipe i=%d\n",i);
				dup2(_outFile?outdis:defaultout,1);
				dup2(fdpipe[i-1][0],0);
				printf("last command output will be in console or file i=%d\n",i);
			}
		}

		_currentSimpleCommand = _simpleCommands[i];
		signal(SIGCHLD,zombie_handler); 
		pid = fork();
		if(pid == 0){
			if(_numberOfSimpleCommands>1){
			if(i<_numberOfSimpleCommands-1)close(fdpipe[i][1]);
			if(i>0) close(fdpipe[i-1][0]);
			}
			execvp(_currentSimpleCommand->_arguments[0],_currentSimpleCommand->_arguments);
		}
		else if(pid > 0){
			if(_numberOfSimpleCommands>1){
			if(i<_numberOfSimpleCommands-1)close(fdpipe[i][1]);
			if(i>0) close(fdpipe[i-1][0]);
			}
			if(!_background){
			int status,x;
			time_t now = time(0);
			x= waitpid(pid,&status,0);
			fprintf(lgfile,"child with id %d was terminated with status %d on %s\n",x,status,ctime(&now));
			}
			//else{
			//}
			}		
	}




	dup2(defaultin,0);
	dup2(defaultout,1);
	dup2(defaulterr,2);
	/*close(outdis);
	close(indis);
	close(errdis);
	close(defaultin);
	close(defaultout);
	close(defaulterr);*/
	//close(fdpipe[1]);
	//close(fdpipe[0]);
	// Add execution here
	// For every simple command fork a new process
	// Setup i/o redirection
	// and call exec

	// Clear to prepare for next command
	clear();
	
	// Print new prompt
	prompt();
}

// Shell implementation

void
Command::prompt()
{
	printf("myshell>");
	fflush(stdout);
}

Command Command::_currentCommand;
SimpleCommand * Command::_currentSimpleCommand;

int yyparse(void);

void setup_term(void) {
    struct termios t;
    tcgetattr(0, &t);
    t.c_lflag &= ~ECHOCTL;
    tcsetattr(0, TCSANOW, &t);
}

int 
main()
{
	//setup_term();
	lgfile = fopen("logfile.txt","a");
	fprintf(lgfile,"Logfile\n");
	sigset( SIGINT, SIG_IGN );
	Command::_currentCommand.prompt();
	yyparse();
	fclose(lgfile);
	return 0;
}