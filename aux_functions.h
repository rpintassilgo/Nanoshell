// Rodrigo Ferreira Pintassilgo - 2191190
//Daniel Píres Patricio - 2191195

#ifndef AUX_FUNCTIONS_H_INCLUDED
#define AUX_FUNCTIONS_H_INCLUDED

int count_chr(const char *string, char chr_to_count);
char* stdout_stderr_redirection(char* command_line, char* cmd_copy_token, char* temporary_cmd, 
                            char* stringOutputFile, char* delimiters, int *enteredVar);
int file_exists(struct gengetopt_args_info args_info);

int is2_before_redirectionSymbol(const char* cmd_line);

void subfunction_for_nano_max();
void executing_nanoShell$();

void remove_spaces(char * command);
void remove_spaces_from_string(char * str);


#endif // AUX_FUNCTIONS_H_INCLUDED
