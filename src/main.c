#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define BUFFER_SIZE	512
#define TOKEN_MAX	50
#define PATH_MAX	512
#define ALIAS_MAX	10
#define HISTORY_MAX	20

// Environment code
char *env_home = NULL;
char *env_path_master = NULL;
char *env_path_current = NULL;

// Delimiters
static const char *delim = " \n";

// Stores the alias names and values
char *alias_key[ALIAS_MAX];
char *alias_value[ALIAS_MAX];

// Stores the number of aliases present
unsigned int alias_count = 0;

// Defines a way of storing a command in history
typedef struct {
	int number; 
	char *string;
} history_t;

// Stores the historical elements
history_t history_value[HISTORY_MAX];

// Stores the number of historical elements
unsigned int history_count = 0;

/* Initialise the command history */
void history_init() {
	// Initialise it to empty for now
	for(int i = 1; i < HISTORY_MAX; i++) {
		history_value[i].number = i;
		history_value[i].string = NULL; 
	}
}

/* Search the alias list for the value key
   
   Params:
   	key - The alias name (e.g. dir)
   	
   Returns:
   	The corresponding value (i.e. what the alias resolves to). If the key isn't
   	found, NULL is returned.
 */
char *alias_get(const char *key) {
	// Check if key value is NULL, if so we can't continue
	if(key == NULL)
		return NULL;
		
	// Iterate through the aliases looking for a match
	for(int i = 0; i < ALIAS_MAX; i++) {
		if(alias_key[i] == NULL)
			// We don't want to compare a NULL value
			continue;
		else if(strcmp(alias_key[i], key) == 0)
			// Found a match, return its corresponding value
			return alias_value[i];
	}
	
	return NULL;
}

/* Add an alias
   
   Params:
   	key - The alias name (e.g. dir)
   	value - What the alias resolves to (e.g. ls -a)
   	
   Returns:
   	If the addition failed, false is returned. Otherwise true.
 */
bool alias_add(const char *key, const char *value) {
	// Check if either values are NULL, if so we can't continue
	if(key == NULL || value == NULL)
		return false;
		
	// Iterate through the aliases looking for a free space
	char *exists = alias_get(key);
	
	if(exists != NULL) {
		// Alias already exists, overwrite the value
		free(exists);
		exists = malloc(strlen(value) + 1);
		strcpy(exists, value);
		
		return true;
	}
	
	for(int i = 0; i < ALIAS_MAX; i++) {
		if(alias_key[i] == NULL) {
			// Found a free space, insert it here
			alias_key[i] = malloc(strlen(key) + 1);
			strcpy(alias_key[i], key);
			alias_value[i] = malloc(strlen(value) + 1);
			strcpy(alias_value[i], value);
			
			alias_count++;
			
			return true;
		}
	}
	
	return false;
}

/* Remove an alias
   
   Params:
   	key - The alias name (e.g. dir)
   
   Returns:
   	If the removal failed, false is returned. Otherwise true.
 */
bool alias_remove(const char *key) {
	// Check if key value is NULL, if so we can't continue
	if(key == NULL)
		return false;
		
	// Iterate through the aliases looking for a match
	for(int i = 0; i < ALIAS_MAX; i++) {
		if(alias_key[i] == NULL)
			// We don't want to compare a NULL value
			continue;
		else if(strcmp(alias_key[i], key) == 0) {
			free(alias_key[i]);
			alias_key[i] = NULL;
			free(alias_value[i]);
			alias_value[i] = NULL;
			
			alias_count--;
			
			return true;
		}
	}
	
	return false;
}

/* Outputs the list of aliases */
void alias_print() {
	if(alias_count == 0) {
		// No aliases present
		printf("You have not set any aliases!\n");
		return;
	}
		
	// There's aliases present, iterate through all valid aliases
	for(int i = 0; i < ALIAS_MAX; i++) {
		if(alias_key[i] != NULL && alias_value[i] != NULL) {
			// Found a valid alias pair
			printf("[%d]: '%s' -> '%s'\n", i, alias_key[i], alias_value[i]);
		}
	}
	
	return;
}

/* Initialise the alias structures */
void alias_init() {
	FILE *alias_file = fopen(".aliases", "r");
	char alias_buffer[BUFFER_SIZE];
	char *alias_tokens[TOKEN_MAX];
	int i = 0;
	
	if(alias_file == NULL) {
		printf("warning: .aliases file missing\n");
		
		// Set all values to NULL since no aliases file exists
		for(i = 0; i < ALIAS_MAX; i++) {
			alias_key[i] = NULL;
			alias_value[i] = NULL;
		}
	}
	else {
		// File exists, parse it accordingly
		while(fgets(alias_buffer, BUFFER_SIZE, alias_file) != NULL) {
			// We've read in a line, lets tokenize it
			alias_tokens[i] = strtok(alias_buffer, delim);
			
			while(alias_tokens[i] != NULL) {
				// Still more to split
				i++;
				alias_tokens[i] = strtok(NULL, delim);
			}
			
			// Do some validity checking...
			if(alias_tokens[0] == NULL)
				continue;
				
			if(strcmp(alias_tokens[0], "alias") != 0)
				continue;
			
			// i represents the number of tokens (beginning at 0)
			if(i < 2)
				// This means that there isn't enough arguments to add an alias
				continue;
			
			// Parsed the line, but we need to form the command2 string as it
			// may contain further arguments (e.g. alias dir ls -a)
			char command2[BUFFER_SIZE] = "";
			
			// We want to skip the first two arguments (i.e. alias <command1>)
			for(int j = 2; j < i; j++) {
				strcat(command2, alias_tokens[j]);
				
				// i represents the number of tokens
				if(j < (i - 1))
					// Space the args out, except the last arg
					strcat(command2, " ");
			}
			
			// Add the alias and reset token counter
			printf("Adding alias: '%s' -> '%s'\n", alias_tokens[1], command2);
			alias_add(alias_tokens[1], command2);
			i = 0;
		}
	}
	
	fclose(alias_file); 
	
	return;
}

/* unalias internal command */
void command_unalias(const char *command) {
	if(alias_get(command) != NULL) {
		// Alias exists, remove it
		if(!alias_remove(command)) {
			fprintf(stderr, "error: unable to remove alias '%s'\n", command);
		}
	}
	else {
		fprintf(stderr, "error: alias '%s' does not exist\n", command);
	}
	
	return;
}

/* alias internal command */
void command_alias(const char *command1, const char *command2) {
	if(command1 == NULL || command2 == NULL) {
		// Print the list of aliases
		alias_print();
	}
	else {
		// Map an alias
		if(alias_count == ALIAS_MAX) {
			// Reached the maximum amount of aliases
			fprintf(stderr, "error: maximum number of alises set\n");
			return;
		}
		if(alias_get(command1) != NULL) {
			// Alias already exists
			printf("warning: overwriting alias '%s'\n", command1);
		}
		if(!alias_add(command1, command2)) {
			// Something went wrong when trying to add the alias
			fprintf(stderr, "error: unable to add alias\n");
		}
	}
	
	return;
}

/* cd internal command */
void command_cd(const char *path) {
	if(chdir(path) == -1)
		printf("%s: no such directory\n", path);
		
	return;
}

/* getpath internal command */
void command_getpath() {
	printf("%s\n", env_path_current);
	
	return;
}

/* setpath internal command */
void command_setpath(const char *path) {
	if(strlen(env_path_current) < strlen(path)) {
		// Not enough space in current path to store new path
		free(env_path_current);
		env_path_current = malloc(strlen(path) + 1);
	}
		
	strncpy(env_path_current, path, strlen(path) + 1);
	setenv("PATH", env_path_current, 1);
	
	return;
}

/* pwd internal command */
void command_pwd() {
	char current_directory[PATH_MAX];
	
	getcwd(current_directory, PATH_MAX);
	printf("%s\n", current_directory);
	
	return;
}

/* help internal command */
void command_help() {
	printf("alias\t print aliases or add an alias\n");
	printf("unalias\t remove an alias\n"); 
	printf("cd\t change current working directory\n");
	printf("getpath\t print system path\n");
	printf("setpath\t set system path\n");
	printf("pwd\t print current working directory\n");
	printf("help\t list the available internal shell commands\n");
	printf("exit\t exit the shell\n");
	
	return;
}

/* Save the alias set to .aliases for alias persistence */
void save_aliases() {
	FILE *alias_file;
	const char *arg1 = "alias";
	
	// Set the working directory to HOME first
	command_cd(env_home);
	
	// Overwrite the old .aliases file
	alias_file = fopen(".aliases", "w");
	
	for(int i = 0; i < alias_count; i++) {
		char *arg2 = alias_key[i];
		char *arg3 = alias_value[i];
		
		if(arg2 == NULL || arg3 == NULL)
			// Record is empty, skip it
			continue;
		
		printf("[DEBUG]: alias = %s %s %s\n", arg1, arg2, arg3);
		fprintf(alias_file, "%s %s %s\n", arg1, arg2, arg3);
	}
	
	fclose(alias_file);
	
	return;
}

/* Save the history for history persistence */
void save_history() {
	// TODO
	return;
}

/* Execute clean up code */
void cleanup() {
	// Reset the PATH variable
	setenv("PATH", env_path_master, 1);
	
	// Save aliases
	save_aliases();
	
	// Save history
	save_history();
	
	return;
}

/* Execute an external process using the arguments provided
   
   Params:
	argc - The number of arguments
	argv - The array of argument strings (argv[0] is the program name)
 */
void execute_process(int argc, char *argv[]) {
	pid_t new_process;

	// fork() a new child process
	new_process = fork();
	
	if(new_process < 0) {
		// Error occurred
		perror("error: fork() failed");
	}
	else if(new_process == 0) {
		// Child process
		printf("[DEBUG]: execute_process(): execvp() called with:\n");
		printf("\targc = %d\n", argc);
		
		for(int i = 0; i <= argc; i++)
			printf("\targv[%d] = %s\n", i, argv[i]);
		
		if(execvp(argv[0], argv) == -1)
			perror("error: execvp() failed");
			exit(1);
		
		exit(0);
	}
	else {
		// Wait for child to complete
		wait(NULL);
		printf("[DEBUG]: execute_process(): child process complete\n");
	}
	
	return;
}

/* Parse the tokenized input and perform the relevant and appropriate operation
   
   Params:
	token_count - The number of tokens in the array
	token_list - The array of token strings
 */
void parse_tokens(int token_count, char *token_list[]) {
	// Use the first token as indication of what to do (e.g. exit, cd, etc.)
	if(strcmp(token_list[0], "cd") == 0) {
		// cd called
		if(token_count == 1)
			// cd called by itself, set to home directory
			command_cd(env_home);
		else if(token_count == 2)
			// cd called with an argument, set to that
			command_cd(token_list[1]);
		else
			printf("usage: cd <dir>\n");
	}
	else if(strcmp(token_list[0], "pwd") == 0) {
		// pwd called
		command_pwd();
	}
	else if(strcmp(token_list[0], "getpath") == 0) {
		// getpath called
		command_getpath();
	}
	else if(strcmp(token_list[0], "setpath") == 0) {
		// setpath called
		if(token_count != 2)
			// Needs to be in format setpath <path>
			printf("usage: setpath <path>\n");
		else
			// token_list[1] = <path>
			command_setpath(token_list[1]);
	}
	else if(strcmp(token_list[0], "history") == 0) {
		// history called
		if(history_count == 0) {
			fprintf(stderr, "error: no history recorded\n");
		}
		else {
			for(int i = 1; i < HISTORY_MAX; i++) {
				if(history_value[i].string != NULL) {
					printf("%d = %s\n",
						history_value[i].number, history_value[i].string);
				}
			}
		}
	}
	else if(strcmp(token_list[0], "alias") == 0) {
		// alias called
		if(token_count >= 3) {
			// Form the arguments into a string
			char command_buffer[BUFFER_SIZE] = "";
			
			// i = 2 because we only want "command2"
			for(int i = 2; i < token_count; i++) {
				strcat(command_buffer, token_list[i]);
				
				if(i < (token_count - 1))
					// append spaces, but not for the last arg
					strcat(command_buffer, " ");
			}
			
			command_alias(token_list[1], command_buffer);
		}
		else if(token_count == 1)
			command_alias(NULL, NULL);
		else
			printf("usage: alias [command1] [command2]\n");
	}
	else if(strcmp(token_list[0], "unalias") == 0) {
		// unalias called
		if(token_count != 2)
			printf("usage: unalias <command>\n");
		else
			command_unalias(token_list[1]);
	}
	else if(strcmp(token_list[0], "exit") == 0) {
		// exit command called
		cleanup();
		exit(0);
	}
	else if(strcmp(token_list[0], "help") == 0) {
		// help command called
		command_help();
	}
	else {
		// Check if command is an alias
		char *alias = alias_get(token_list[0]);
		
		if(alias != NULL) {
			// Don't want to alter original contents
			char *alias_buffer = malloc(strlen(alias) + 1);
			strcpy(alias_buffer, alias);
			
			// Command is an alias, lets split it up
			int count = 0;
			char *alias_args[TOKEN_MAX];
			
			alias_args[count] = strtok(alias_buffer, delim);
			
			while(alias_args[count] != NULL) {
				count++;
				alias_args[count] = strtok(NULL, delim);
			}
			
			// Execute command
			parse_tokens(count, alias_args);
			
			free(alias_buffer);
			return;
		}
		
		// An unsupported internal command was called, we must assume it's an 
		// external command
		execute_process(token_count, token_list);
	}
	
	return;
}

int main(int argc, char *argv[]) {
	// User input buffer
	char buffer[BUFFER_SIZE];
	char full_command[BUFFER_SIZE];
	bool history_invoke = false;
	
	// Store tokens here (according to specification 50 tokens is reasonable)
	char *token_list[TOKEN_MAX];
	int token_count = 0;
	
	// Get the users HOME directory and set the current directory to that
	if((env_home = getenv("HOME")) == NULL)
		printf("warning: HOME variable undefined\n");
	else {
		printf("[DEBUG]: HOME = %s\n", env_home);
		chdir(env_home);
	}
	
	// Get the users PATH variable and set the shell PATH to that
	if((env_path_master = getenv("PATH")) == NULL)
		printf("warning: PATH variable undefined\n");
	else {
		printf("[DEBUG]: PATH = %s\n", env_path_master);
		env_path_current = env_path_master;
	}
	
	// Load aliases
	printf("Initialising aliases:\n");
	alias_init();
	
	// Load history
	printf("Initialising history...\n");
	history_init();
	
	// Start the shell
	while(1) {
		printf("$ ");
		
		if(fgets(buffer, BUFFER_SIZE, stdin) == NULL) {
			// stdin stream closed, can't continue, end loop
			perror("error: stdin stream closed");
			break;
		}
		
		// Copy buffer over to save the full command
		strcpy(full_command, buffer);
		strtok(full_command, "\n"); // remove trailing new line
		
		// Tokenize the user input and store the tokens in an array for easy 
		// parsing
		token_list[token_count] = strtok(buffer, delim);
		
		while(token_list[token_count] != NULL) {
			// There's still more tokens to get
			token_count++;
			token_list[token_count] = strtok(NULL, delim);
		}
		
		for(int i = 0; i <= token_count; i++)
			printf("token_list[%d] = %s\n", i, token_list[i]);
		
		printf("token_count = %d\n", token_count);
		
		if(token_count == 0)
			// Can't parse nothing...
			continue;
			
		// We're using a circular array, so when we hit the max go back to start
		int history_pos = (history_count + 1) % HISTORY_MAX;
		
		if(full_command[0] == '!' && full_command[1] == '!') {
			// Previous history invokation
			if(history_count == 0) {
				// There's no recorded history
				printf("There's no commands in the history\n");
			}
			else {
				// Parse the previous command
				char *prev = history_value[history_pos - 1].string;
				strcpy(buffer, prev);
				
				history_invoke = true;
			}
		}
		else if(full_command[0] == '!' && !(full_command[1] == '!')) {
			// Possibly a number passed for history invokation
			char *history_str_number = full_command + 1;
			int history_number = atoi(history_str_number);
			
			if(history_number == 0 || (history_number > history_count)) {
				// Failed - either invalid number passed or 0
				fprintf(stderr, "error: invalid history number\n");
			}
			else {
				// Got a number, so attempt to fetch the history
				char *fetch = history_value[history_number % HISTORY_MAX].string;
				strcpy(buffer, fetch);
				
				history_invoke = true;
			}
		}
		
		if(history_invoke) {
			// Buffer has been altered with a new command
			// NULL-ify the token_list first to remove existing elements
			for(int i = 0; i < token_count; i++)
				token_list[i] = NULL;
					
			// Place new instruction into token_list
			token_count = 0;
			token_list[token_count] = strtok(buffer, delim);
			
			while(token_list[token_count] != NULL) {
				// There's still more tokens to get
				token_count++;
				token_list[token_count] = strtok(NULL, delim);
			}
		}
			
		// No matter what the command, we track it anyway
		history_count++;
		
		// Copy history position over to history element
		history_value[history_pos].number = history_count;
		
		// Copy string from buffer over to history element
		history_value[history_pos].string = malloc(strlen(full_command) + 1);
		strcpy(history_value[history_pos].string, full_command);
		
		// Now parse the token(s) and reset the counter
		parse_tokens(token_count, token_list);
		token_count = 0;
		history_invoke = false;
	}
	
	// Execute relevant clean up code
	cleanup();
	
	return 0;
}

