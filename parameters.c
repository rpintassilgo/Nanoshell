// Rodrigo Ferreira Pintassilgo - 2191190
//Daniel Píres Patricio - 2191195

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "debug.h"
#include "memory.h"
#include "args.h"
#include "parameters.h"
#include "aux_functions.h"
#include <sys/stat.h> // We're calling this library because we're going to check if a file is empty. 
					  //Of course, we could pick a different approach and fseek to the end of the file and then check if ftell returns 0.



void file_parameter(struct gengetopt_args_info args_info) {
	FILE *fptr = NULL;	 
	fptr = fopen(args_info.file_arg,"rb"); //It will open the file indicated by the user
	char *token; // this pointer is needed for the strtok()

	if (fptr == NULL) { //check for possible errors
		ERROR(1,"'%s': CANNOT PROCESS FILE\n",args_info.file_arg);
	}

	else {
		struct stat stat_file;
		stat(args_info.file_arg, &stat_file); //this function return information about a file and then store it inside a structure called stat
		if(stat_file.st_size == 0){ // st_size stores the total size (in bytes)   ; We're checking if the file is empty (0 bytes)
			ERROR(3,"The file is empty!\n");
		}


		char *line; //pointer to read the whole line before '\n'
		size_t len = 0;
		pid_t id; 

		int n_lines = 0; // number of lines
		
		while (getline(&line,&len,fptr) != -1 ) { //looping lines from the file
			line[strlen(line)-1] = '\0'; //it will remove '\n' from each line
			int i=0;
			n_lines++;

			char *stringOutputFile = NULL;; // this string is going to contain the name of the file for > , >> , 2> , 2>>
			char *temporary_cmd = NULL;

			int entered_v = 0;  //already explained in "subfunction_for_nano_max"
			int entered_vv = 0;
			int entered_2v = 0;
			int entered_2vv = 0;


				char *cmd_copy_token = strdup(line);
				

				if ( (strchr(line, '>') != NULL) && (count_chr(line,'>') == 1) && (is2_before_redirectionSymbol(line) == 0) ) {  
					stringOutputFile = stdout_stderr_redirection(line,cmd_copy_token,temporary_cmd,stringOutputFile,">",&entered_v);

				}
				else if ( (strstr(line, ">") != NULL) && (count_chr(line,'>') == 2) && (is2_before_redirectionSymbol(line) == 0) ) {
					stringOutputFile = stdout_stderr_redirection(line,cmd_copy_token,temporary_cmd,stringOutputFile,">",&entered_vv);
				}
				else if ( (strstr(line, "2>") != NULL) && (count_chr(line,'>') == 1) && (is2_before_redirectionSymbol(line) == 1) ) {
					stringOutputFile = stdout_stderr_redirection(line,cmd_copy_token,temporary_cmd,stringOutputFile,"2>",&entered_2v);
				}
				else if ( (strstr(line, "2>") != NULL) && (count_chr(line,'>') == 2) && (is2_before_redirectionSymbol(line) == 1) ) {
					stringOutputFile = stdout_stderr_redirection(line,cmd_copy_token,temporary_cmd,stringOutputFile,"2>",&entered_2vv);
				}  //this is basically doing the same, and it is already explained inside "subfunction_for_nano_max()"
			


			token = strtok(line," "); // This function divides a string into several parts using a delimiter.
			char *lineTokens[200] = {NULL}; // This is being inicialized as NULL so it will make our job easier with execvp



			
			while (token != NULL) { //loop of words in a line
				lineTokens[i++] = token; // storing each token inside the matrix
				//each time the loop repeats, the counter adds +1 to 'i' so it will never overwrite anything
				token = strtok(NULL," "); //since we're looping while(token != NULL)
                   							//we want to make sure it wont loop forever so we need to make sure.
                                            //In this case token = strtok(NULL, ",") will do the same thing as token = NULL
			}                                //that would stop the loop






			id = fork(); //it will create a child to run execvp. This is needed because execvp replaces the image of the current process
			if (id < 0){ //check for errors
				ERROR(2,"fork() did not work!\n");
			}
			if(id == 0){ //child
				if(entered_v == 1){
					printf("[INFO] stdout redirected to '%s'\n",stringOutputFile); //print the file name
					printf("[command #%d]: %s\n",n_lines,cmd_copy_token); // the number of the command and the command itself
					freopen(stringOutputFile,"w",stdout); // https://stackoverflow.com/Questions/1908687/how-to-redirect-the-output-back-to-the-screen-after-freopenout-txt-a-stdo
					freopen ("/dev/tty", "a", stdout); // to simplify we redirected stdout to "/dev/tty" ; we got that idea from that website (print the result of the command)
					free(stringOutputFile);  			//freopen is already explained inside "subfunction_for_nano_max"
					free(cmd_copy_token);          
				}
				if(entered_vv == 1){
					printf("[INFO] stdout redirected to '%s'\n",stringOutputFile);
					printf("[command #%d]: %s\n",n_lines,cmd_copy_token);  //explained above
					freopen(stringOutputFile,"a",stdout);
					freopen ("/dev/tty", "a", stdout);
					free(stringOutputFile);
					free(cmd_copy_token);

				}
				if(entered_2v == 1){
					printf("[INFO] stderr redirected to '%s'\n",stringOutputFile);
					printf("[command #%d]: %s\n",n_lines,cmd_copy_token); // explained above
					freopen(stringOutputFile,"w",stderr);
					freopen ("/dev/tty", "a", stdout);
					free(stringOutputFile);
					free(cmd_copy_token);
				}
				if(entered_2vv == 1){
					printf("[INFO] stderr redirected to '%s'\n",stringOutputFile);
					printf("[command #%d]: %s\n",n_lines,cmd_copy_token);    //explained above
					freopen(stringOutputFile,"a",stderr);
					freopen ("/dev/tty", "a", stdout);
					free(stringOutputFile);
					free(cmd_copy_token);

				}
				execvp(lineTokens[0],lineTokens);
			} // everything outside the loop is running as the parent(main) process
			wait(NULL); // the main process waits for the child process 
			  			//(after the child process terminates, the main process will be resumed)
					
		}
    
		fclose(fptr);
		free(line);
		//getline() uses realloc to resize the input buffer so the user will need to free the memory!
    }	
}

void max_parameter(struct gengetopt_args_info args_info) {

	while (1) { 	// It is an infinite loop that will be executed until a break statement is issued explicitly. Or 1

		printf("[INFO] terminates after %d commands\n",args_info.max_arg);

		for (int k=0; k<args_info.max_arg; k++) {
			subfunction_for_nano_max();
		}
		printf("[END] Executed %d commands (-m %d).\n",args_info.max_arg,args_info.max_arg);
		exit(0);	
	}
}

void help_parameter() {
	printf("\t\t\t\tHELP DOCUMENTATION\n");
	printf("OPTIONS:\n");
	printf("\t|OPTIONAL| --file/-f: This option executes any command line indicated in the text file!\n");
	printf("\t|OPTIONAL| --no-help/-h: This option prints this help menu!\n");
	printf("\t|OPTIONAL| --max/-m: This option applies the maximum number of command executions carried out by nanoShell!\n");
	printf("\t|OPTIONAL| --signalfile/-s: This option creates the signal.txt file that contains commands that will enable the user to send signals to the nanoShell!\n");
	printf("\nMORE INFORMATION:\n");
	printf("\n\tRodrigo Pintassilgo - 2191190\n");
	printf("\tDaniel Patrício - 2191195\n");
}

void signalfile_parameter() {         

	FILE *fptr = NULL;
	char filename[] = "signal.txt";	 
	char signals[3] [9] = {"-SIGINT", "-SIGUSR1", "-SIGUSR2"};
	
	fptr = fopen(filename,"w"); //It will open the file indicated by the user

	if (fptr == NULL) { //check for possible errors
		ERROR(1,"'%s': CANNOT PROCESS FILE\n",filename);
	}
	else {

		for (int j=0;j<3;j++) {   
			int check = fprintf(fptr,"kill %s %d\n",signals[j],getpid());
			if(check < 0){
				printf("FALHOU\n");
			}
		}

		printf("[INFO] created file '%s'\n",filename);

		fclose(fptr); 

		executing_nanoShell$(); 

			
	}
}









































