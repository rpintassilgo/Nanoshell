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
#include "aux_functions.h"
#include <sys/stat.h> // We're calling this library because we're going to check if a file is empty. 
					  //Of course, we could pick a different approach and fseek to the end of the file and then check if ftell returns 0.



int count_chr(const char *string, char chr_to_count){
	int repetions = 0;
	for(int i=0;i<(int)strlen(string);i++){ // in this case,there is no problem if I cast long unsigned int to int
		if( string[i] == chr_to_count){ // because the string will never be bigger than the max value that int can store!
			repetions++;
		}       //basically, this will loop every char of any specified string and count the selected char , and then, return the number of repetions
	}

	return repetions;

}

char *stdout_stderr_redirection(char* command_line, char* cmd_copy_token, char* temporary_cmd, 
    char* stringOutputFile, char* delimiters, int *enteredVar){
		int r=0; // it will count all the rounds that while loop will do before it ends.
				cmd_copy_token = strtok_r(command_line,delimiters,&command_line);
				while(cmd_copy_token != NULL){
 					 if(r == 0){ //if it is the first time looping
						temporary_cmd = malloc(strlen(cmd_copy_token)); //this malloc will allocate memory for temporary_cmd with the size of cmd_copy_token
						if(temporary_cmd == NULL){						// I didn't indicate the size of char since the size of char is 1
							ERROR(1,"Insuficient Memory\n");  // in case malloc fails
						}else{
							strcpy(temporary_cmd,cmd_copy_token);		//strcpy will copy cmd_copy_token to the new string (temporary_cmd)	
						}

					 } else{ // if it is not the first time looping (example: second round)
					 	stringOutputFile = malloc(strlen(cmd_copy_token)); // since it is a string size is 1 (no need to indicate that)
					 	if(stringOutputFile == NULL){
							 ERROR(1,"Insuficient Memory\n"); // in case malloc fails
						}
						else{
							strcpy(stringOutputFile,cmd_copy_token); // it will copy cmd_copy_token to the string with the name of the file that will store the redirected output
							remove_spaces_from_string(stringOutputFile); // this function will remove any spaces that the string may contain
						}
					 } 
					cmd_copy_token = strtok_r(NULL,delimiters,&command_line); // the loop wont loop forever 
					r++; // rounds counter
				}
				//reseting the string before copying the command without the file information
				for(int p=0;p<(int)strlen(command_line);p++){// in this case,there is no problem if i cast long unsigned int to int
					command_line[p] = NULL;	// because the string will never be bigger than the max value that int can store!
				}
				strcpy(command_line,temporary_cmd); // it will copy the command without the redirection intructions (>,>>,2>,2>>) and the file name
				free(temporary_cmd);

				*enteredVar = 1; // this pointer will change the value of those variables that we created to make our job easier when using 'freopen'

				return stringOutputFile; //since we are allocating memory inside a function and the string is declared outside the function, we need
										//to return 'stringOutputFile' or the string outside of the function will not know where the string is stored in the memory

}

int file_exists(struct gengetopt_args_info args_info) { //this function will verify the existence of the file by trying to open it
	FILE *fptr;
	
	if((fptr = fopen(args_info.file_arg,"r")) != NULL) {
		fclose(fptr);
		return 1; //if the file exists, it will return 1 and close the file
	}
	else {
		return 0; // if the file doesn't exist, it will return 0
	}
	
}

int is2_before_redirectionSymbol(const char* cmd_line){ // this function will be used to check if the character before the first '>' is '2'
	int is = 0; // 1 - it is '2' ; 0 - it isn't '2'
	int i = 0; // counter
	for(i=0;i<(int)strlen(cmd_line);i++){ //this 'for loop' will loop all the chars in the string and if it finds the char '>', it stops/breaks the loop 
		if(cmd_line[i] == '>')			// 'i' will contain the position of the first '>'
			break;
	}

	if (cmd_line[i-1] == '2'){   // then we will check if the previous char is '2'
		is = 1; // it is
	} else{
		is = 0; // it isn't
	}

	return is; 
}



void subfunction_for_nano_max(){
		char *command_line; //pointer to read the whole line before '\n'
		size_t len = 0;
    	char* ready_cmdline[100];    // User command

			
		printf("nanoShell$ "); // Print shell prompt

		if(!getline(&command_line,&len,stdin)) {		
        	exit(EXIT_FAILURE);
		}

		command_line[strlen(command_line) -1] = '\0';

    	if(strcmp(command_line, "bye")==0){            // Check if command is bye & returns 0 if strings are the same
        	printf("[INFO] bye command detected. Terminating nanoShell\n");
			exit(0);
    	}
		else if (strchr(command_line, '*') != NULL || strchr(command_line, '?') != NULL || 
			strchr(command_line, '|') != NULL || strchr(command_line, '"') != NULL || 
			strstr(command_line,"'") != NULL) { // it finds the first occurrence of a character in string

			remove_spaces(command_line); // it removes spaces at the beginning of the string and at the end of the string, so it will 'printf' it without any spaces
		}
		else {

			char *token;                  //split command into separate strings

			char *stringOutputFile = NULL;; // this string is going to contain the name of the file (for > , >> , 2> , 2>>)
			char *cmd_copy_token = strdup(command_line); // strdup is used to create string copies
			char *temporary_cmd = NULL;

			int entered_v = 0; // these variables will make our job easier when using freopen, since if they entered any if..else we will know
			int entered_vv = 0; // basically,
			int entered_2v = 0; // if the command_line has ">", entered_v will be 1 ; if the command_line has ">>", entered_vv will be 1 
			int entered_2vv = 0;//	if the command_line has "2>", entered_2v will be 1 ; if the command_line has "2>>", entered_2vv will be 1 

				//search for the first occurrence of '>' ; check if there is only 1 '>' ; check if the char before first '>' is not '2'
			if ( (strchr(command_line, '>') != NULL) && (count_chr(command_line,'>') == 1) && (is2_before_redirectionSymbol(command_line) == 0) ) {
				stringOutputFile = stdout_stderr_redirection(command_line,cmd_copy_token,temporary_cmd,stringOutputFile,">",&entered_v);

			}  //search for the first occurrence of '>'  ; check if there is only 2 '>'  ; check if the char before the first '>' is not '2'
			else if ( (strstr(command_line, ">") != NULL) && (count_chr(command_line,'>') == 2) && (is2_before_redirectionSymbol(command_line) == 0) ) {
				stringOutputFile = stdout_stderr_redirection(command_line,cmd_copy_token,temporary_cmd,stringOutputFile,">",&entered_vv); 
			}	//search for the first occurrence of '2' and '>'	; check if there is only 1 '>' ; check if the char before the first '>' is '2'
			else if ( (strstr(command_line, "2>") != NULL) && (count_chr(command_line,'>') == 1) && (is2_before_redirectionSymbol(command_line) == 1) ) {
				stringOutputFile = stdout_stderr_redirection(command_line,cmd_copy_token,temporary_cmd,stringOutputFile,"2>",&entered_2v);
			}	//search for the first occurrence of '2' and '>' ; check if there is only 2 '>' ; check if the char before the first '>' is '2'
			else if ( (strstr(command_line, "2>") != NULL) && (count_chr(command_line,'>') == 2) && (is2_before_redirectionSymbol(command_line) == 1) ) {
				stringOutputFile = stdout_stderr_redirection(command_line,cmd_copy_token,temporary_cmd,stringOutputFile,"2>",&entered_2vv);
			} // those functions inside each if...else sentence will divide the command line between the bash command and the file redirection information.
				// stringOutputFile will be used to store the file name and command_line will be erased (filled with NULLs) and then it will be used to store the bash command
			if ( strchr(command_line, '>') != NULL){ //it will free cmd_copy_token
				free(cmd_copy_token);
			}

    		token = strtok(command_line," "); // it uses " " as delimited to divide every token
    		long unsigned int i=0;

    		while(token != NULL){
        		ready_cmdline[i++] = token;    	// Puts each token in vector
        		token = strtok(NULL," ");
    		}

    		ready_cmdline[i]=NULL;  //set last value to NULL for execvp

			 
    		pid_t pid = fork();              //fork child
    		if (pid == 0) {               //Child
				if(entered_v == 1){ // if >
					printf("[INFO] stdout redirected to '%s'\n",stringOutputFile);
					freopen(stringOutputFile,"w",stdout); //redirect stdout to the file stored inside the string; if the file exists, the content will be overwritten
					free(stringOutputFile); // free the string		;								if the file doesn't exist, the file will be created
				}
				if(entered_vv == 1){ // if >>
					printf("[INFO] stdout redirected to '%s'\n",stringOutputFile); 
					freopen(stringOutputFile,"a",stdout);	//redirect stdout to the file stored inside the string; if the file exists, it will append the content																
					free(stringOutputFile); // free the string	;												if the file doesn't exist, the file will be created

				}
				if(entered_2v == 1){ // if 2>
					printf("[INFO] stderr redirected to '%s'\n",stringOutputFile);
					freopen(stringOutputFile,"w",stderr); //redirect stderr to the file stored inside the string; if the file exists, the content will be overwritten
					free(stringOutputFile);					//if the file doesn't exist, the file will be created
				}
				if(entered_2vv == 1){ // if 2>>
					printf("[INFO] stderr redirected to '%s'\n",stringOutputFile);
					freopen(stringOutputFile,"a",stderr); //redirect stderr to the file stored inside the string; if the file exists, it will append the content	
					free(stringOutputFile);															//if the file doesn't exist, the file will be created

				}
        		execvp(ready_cmdline[0],ready_cmdline);        //The first argument of execvp is the file you want to execute, 
        		ERROR(4,"Child process could not do execvp\n"); //and the second argument is an array of null-terminated strings that represent the appropriate arguments to the file
    		}													// execvp is a system call and it let us run bash commands;S the image of the process will be overwritten
			else if (pid > 0) {                    //Parent
        		wait(NULL); //Waits for the child process to finish	
    		}
			else {
				ERROR(3,"Fork() execution error\n");
			}
		}	
}





void executing_nanoShell$() {
	while (1) { 	// It is an infinite loop that will be executed until a break statement is issued explicitly
		subfunction_for_nano_max();
	}
}



void remove_spaces(char * command) {    

	//ex: nanoShell$ ls "a.txt"___________________________________ / (_) -> spaces
	//ex: nanoShell$ ___________________________________ls "a.txt" / (_) -> spaces
	//ex: nanoShell$ _____________________ls "a.txt"___________________ / (_) -> spaces

    int index_last, i, index_first, j;
	//index_last -> index for the last spaces
	//index_first -> index for the first spaces

    //Set default index
    index_last = 0;

    //Find last index of non-white space character 
    i = 0;
    while(command[i] != '\0') {
        if(command[i] != ' ' && command[i] != '\t' && command[i] != '\n') {
            index_last = i;
        }
		i++;
    }

    //Mark next character to last non-white space character as NULL
    command[index_last + 1] = '\0';
	//printf("[ERROR] Wrong request '%s'\n",command); 

	//Find first index of non-white space character 
	j = 0;

	while(command[j] != '\0') {
        if(command[j] != ' ' && command[j] != '\t' && command[j] != '\n' && command[j] != '\r' && command[j] != '\f' && command[j] != '\v') {
            index_first = j;
			//printf("i -> %d\n", index_first); //teste
			break;
        }
		j++;
    }
		
	int len = strlen(command);
	char *str = /*(char *)*/ malloc(len + 1);
	if(str == NULL){
		ERROR(1,"Insuficient Memory\n");
	} else{
		int y;
		for (y = index_first; y <= len; ++y) {	
		str[y-index_first] = command[y];
	}

	printf("[ERROR] Wrong request '%s'\n",str);

	}

	free(str);
}

void remove_spaces_from_string(char * str){ 
    char * back = str;				//https://stackoverflow.com/questions/1726302/removing-spaces-from-a-string-in-c
    do{								// this function is basically a function from stackoverflow
        if(*back != ' '){			//before checking this function, my idea was to use again strtok()
			*str++ = *back;			// obviously overcomplicating, this function is the opposite (oversimplifying)
		} //if the position isn't a space it will store it inside str and move one position
    }while(*back++ != '\0'); // it will loop back string until it reaches '\0'
}





