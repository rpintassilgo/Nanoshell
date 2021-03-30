// Rodrigo Ferreira Pintassilgo - 2191190
//Daniel Píres Patricio - 2191195

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
//#include <time.h>
//#include <assert.h>

#include "debug.h"
#include "memory.h"
#include "args.h"
#include "aux_functions.h"
#include "parameters.h"

// functions that are going to handle the signal!
void handle_signal(int signal, siginfo_t *siginfo, void *context);

void handle_signal(int signal, siginfo_t *siginfo, void *context){
	int aux;
	// it will copy from the global variable errno
	aux = errno;

	if (signal == SIGUSR1)
	{
		printf("Received signal (%d)\n", signal);
		//printf("This program is running since %s!\n",current_time);

	}
	if (signal == SIGUSR2)
	{
		printf("Received signal (%d)\n", signal);
		
	}
	if (signal == SIGINT)
	{
		printf("Received signal (%d)\n", signal);
		printf("\tPID: %ld\n", (long)siginfo->si_pid); //printing PID
		exit(0);
	}

	
	errno = aux; // restores value of errno
} 

int main(int argc, char *argv[])
{
	//time_t time_struct = time(NULL);
  	//struct tm current_time = *localtime(&time_struct);
	//char time_stdout[64];
   // assert(strftime(time_stdout, sizeof(time_stdout), "%c", current_time)); 
  	
	//https://stackoverflow.com/questions/1442116/how-to-get-the-date-and-time-values-in-a-c-program

	struct gengetopt_args_info args_info; // It declares a structure ggo arguments

	if (cmdline_parser(argc, argv, &args_info) != 0)
	{ // It tests which parameters were passed
		exit(1);
	}

  // SIG ----------------
 	struct sigaction act;
		

	act.sa_sigaction = handle_signal; 	// Define the signal response routine
	sigemptyset(&act.sa_mask);  	// mask without signals -> it doesn't block signals          	
	act.sa_flags = SA_SIGINFO; 	//  retrieve blocking calls

	// This will capture the signal
	if(sigaction(SIGINT, &act, NULL) < 0){
		ERROR(3, "sigaction  - SIGINT");
	}
	if(sigaction(SIGUSR1, &act, NULL) < 0){
		ERROR(3, "sigaction (sa_handler) - ???");
	}
	if(sigaction(SIGUSR2, &act, NULL) < 0){
		ERROR(3, "sigaction (sa_handler) - ???");
	}
 
 // -------------------------------
	if (args_info.file_given)
	{ // If the file argument is given allocates memory for the string

		if (args_info.no_help_given || args_info.max_given || args_info.signalfile_given)
		{
			printf("-f/--file option isn't compatible with any other command line options!\n");
		}
		else
		{
			int exists = file_exists(args_info);
			if (exists == 0)
			{
				ERROR(1, "cannot open file %s -- ", args_info.file_arg);
			}
			else
			{
				file_parameter(args_info);
			}
		}
	}
	else if (args_info.max_given)
	{
		if (args_info.no_help_given || args_info.file_given || args_info.signalfile_given)
		{
			printf("-m/--max option isn't compatible with any other command line options!\n");
		}
		else
		{
			if (args_info.max_arg <= 0)
			{
				ERROR(2, "invalid value '%d' for -m/--max.\n", args_info.max_arg);
			}
			else
			{
				max_parameter(args_info);
			}
		}
	}
	else if (args_info.no_help_given)
	{
		help_parameter();
	}
	else if (args_info.signalfile_given)
	{
		signalfile_parameter();
	}
	else if (!args_info.file_given && !args_info.no_help_given && !args_info.max_given && !args_info.signalfile_given && (argv[1] == NULL))
	{

		executing_nanoShell$();
	}
	else
	{
		printf("Invalid argument for nanoShell$! \n");
	}

	cmdline_parser_free(&args_info); // Releases resources allocated to gengetopt

	return 0;
}