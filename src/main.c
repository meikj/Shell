/*
 * main.c
 *
 * CS210 Semester 2 Shell Project
 *
 * Authors:
 *	Mark Anderson <mark.anderson@strath.ac.uk>
 *	Andrew Logan <andrew.logan@strath.ac.uk>
 *	John Meikle <john.meikle@strath.ac.uk>
 *
 * Version:
 *	0.9-stage9
 *
 */

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
static const char *delim = " \t\n";

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
	FILE *history_file;
	char history_buffer[BUFFER_SIZE];
	int max = 0;

	history_file = fopen(".hist_list", "r");

	if(history_file == NULL) {
		// Initialise the history to empty
		for(int i = 0; i < HISTORY_MAX; i++) {
			history_value[i].number = i;
			history_value[i].string = NULL; 
		}

		return;
	}

	while(fgets(history_buffer, BUFFER_SIZE, history_file) != NULL) {
		// Line has been read into buffer
		// Only need to extract in format of <number> <command>

		char *number_str = strtok(history_buffer, delim);
		int number = atoi(number_str);

		if(number <= 0) {
			// Skip the particular entry if the number is invalid
			fprintf(stderr, "error: invalid entry in .hist_list, skipping...\n");
			continue;
		}

		// Number extraction seems to have went well, continue...
		char command_buffer[BUFFER_SIZE] = "";
		char *command = strtok(NULL, delim);

		if(command == NULL) {
			// Skip the particular entry if the number is invalid
			fprintf(stderr, "error: invalid entry in .hist_list, skipping...\n");
			continue;
		}

		do {
			// Concatenate the command(s) to the full command string
			strcat(command_buffer, command);
			strcat(command_buffer, " "); // space out args
			command = strtok(NULL, delim);
		} while(command != NULL);

		// Everything seems to have went well and we have formed a history entry
		// Insert history entry into the history array
		int history_pos = number % HISTORY_MAX;

		printf("\tAdding history: %d %s\n", number, command_buffer);

		history_value[history_pos].number = number;
		history_value[history_pos].string = malloc(strlen(command_buffer) + 1);
		strcpy(history_value[history_pos].string, command_buffer);

		history_count++;

		// Check if number is max so we can keep track of highest history number
		if(number > max)
			max = number;
	}

	// Set the history_count to the highest history number so as to preserve the number count
	history_count = max;

	fclose(history_file);

	return;
}

/* Search the alias list for the value key
   
   Params:
   	key - The alias name (e.g. dir)
   	
   Returns:
   	The corresponding value (i.e. what the alias resolves to). If the key isn't
   	found, NULL is returned.
 */
char *alias_get(const char *key) {
	// Check if key value is NULL, if so we can't continue, so return NULL
	if(key == NULL)
		return NULL;
		
	// Iterate through the aliases looking for a match
	for(int i = 0; i < ALIAS_MAX; i++) {
		if(alias_key[i] == NULL)
			// We don't want to compare a NULL value, so skip this iteration
			continue;
		else if(strcmp(alias_key[i], key) == 0)
			// Found a match, return its corresponding value
			return alias_value[i];
	}
	
	return NULL;
}

/* Add an alias to the alias list
   
   Params:
   	key - The alias name (e.g. dir)
   	value - What the alias resolves to (e.g. ls -a)
   	
   Returns:
   	If the addition failed, false is returned. Otherwise true is returned.
 */
bool alias_add(const char *key, const char *value) {
	// Check if either values are NULL, if so we can't continue
	if(key == NULL || value == NULL)
		return false;
		
	// Iterate through the aliases looking for a free space
	char *exists = alias_get(key);
	
	// Check if alias exists
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

/* Remove an alias from the alias list
   
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
			// We don't want to compare a NULL value, so skip this iteration
			continue;
		else if(strcmp(alias_key[i], key) == 0) {
			// We found a match, free the memory and reset to NULL
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

/* Output the list of aliases */
void alias_print() {
	if(alias_count == 0) {
		// No aliases present
		printf("You have not set any aliases!\n");
		return;
	}
		
	// There's aliases present, iterate through all valid aliases
	for(int i = 0; i < ALIAS_MAX; i++) {
		if(alias_key[i] != NULL && alias_value[i] != NULL)
			// Found a valid alias pair, so output the mapping
			printf("[%d]: '%s' -> '%s'\n", i, alias_key[i], alias_value[i]);
	}
	
	return;
}

/* Initialise the alias structures */
void alias_init() {
	FILE *alias_file;
	char alias_buffer[BUFFER_SIZE];
	char *alias_tokens[TOKEN_MAX];
	int token_count = 0;

	alias_file = fopen(".aliases", "r");
	
	if(alias_file == NULL) {
		printf("warning: .aliases file missing\n");
		
		// Set all values to NULL since no aliases file exists
		for(int i = 0; i < ALIAS_MAX; i++) {
			alias_key[i] = NULL;
			alias_value[i] = NULL;
		}

		return;
	}
	else {
		// File exists, parse it accordingly
		while(fgets(alias_buffer, BUFFER_SIZE, alias_file) != NULL) {
			// We've read in a line, lets tokenize it
			alias_tokens[token_count] = strtok(alias_buffer, delim);
			
			while(alias_tokens[token_count] != NULL) {
				// Still more to split
				token_count++;
				alias_tokens[token_count] = strtok(NULL, delim);
			}
			
			// Do some validity checking...
			if(alias_tokens[0] == NULL)
				continue;
				
			if(strcmp(alias_tokens[0], "alias") != 0)
				continue;
			
			if(token_count < 2)
				// This means that there isn't enough arguments to add an alias
				continue;
			
			// We've tokenized the line from the file, but we need to form the
			// arguments into one string (i.e. command2), so let's do that
			char command2[BUFFER_SIZE] = "";
			
			// We want to skip the first two arguments (i.e. alias <command1>)
			for(int i = 2; i < token_count; i++) {
				strcat(command2, alias_tokens[i]);
				
				if(i < (token_count - 1))
					// Space the args out, except the last arg
					strcat(command2, " ");
			}
			
			// Add the alias and reset token counter
			printf("\tAdding alias: '%s' -> '%s'\n", alias_tokens[1], command2);
			alias_add(alias_tokens[1], command2);
			token_count = 0;
		}
	}
	
	fclose(alias_file); 
	
	return;
}

/* history internal command */
void command_history() {
	// Output the history in ascending numerical order

	// An example ordering may be:
	// 20, 21, 22, 3, 4, 5, ..., 18, 19
	// Split it up by iterating backwards and checking for a greater value, this
	// becomes the index_split (i.e. seperates the new from the old)

	int index_split = 0;
	int max = history_value[HISTORY_MAX - 1].number;

	// Iterate through from right to left looking for the first greater value
	for(int i = HISTORY_MAX - 1; i >= 0; i--) {
		if(history_value[i].number > max) {
			// Found the "break point"
			index_split = i;
			break;
		}
	}

	// Output the smaller section
	for(int i = index_split + 1; i < HISTORY_MAX; i++) {
		if(history_value[i].string != NULL) {
			printf("%d = %s\n",
				history_value[i].number, history_value[i].string);
		}
	}

	// If index_split is not 0 then that means there is a split somewhere
	if(index_split != 0) {
		// Output the larger section
		for(int i = 0; i <= index_split; i++) {
			if(history_value[i].string != NULL) {
				printf("%d = %s\n",
					history_value[i].number, history_value[i].string);
			}
		}
	}

	return;
}

/* unalias internal command */
void command_unalias(const char *command) {
	if(alias_get(command) != NULL) {
		// Alias exists, remove it
		if(!alias_remove(command))
			fprintf(stderr, "error: unable to remove alias '%s'\n", command);
	}
	else
		fprintf(stderr, "error: alias '%s' does not exist\n", command);
	
	return;
}

/* alias internal command 
   
   To output the alias list, NULL should be passed as the argument(s)
 */
void command_alias(const char *command1, const char *command2) {
	if(command1 == NULL || command2 == NULL)
		// Print the list of aliases
		alias_print();
	else {
		// Map an alias
		if(alias_count == ALIAS_MAX) {
			// Reached the maximum amount of aliases
			fprintf(stderr, "error: maximum number of alises set\n");
			return;
		}

		if(alias_get(command1) != NULL)
			// Alias already exists
			printf("warning: overwriting alias '%s'\n", command1);

		if(!alias_add(command1, command2))
			// Something went wrong when trying to add the alias
			fprintf(stderr, "error: unable to add alias\n");
	}
	
	return;
}

/* cd internal command */
void command_cd(const char *path) {
	if(chdir(path) == -1)
		fprintf(stderr, "%s: no such directory\n", path);
		
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
		// Not enough space in current path to store the new path, so free
		// the existing memory and allocate a new portion that will fit the
		// new path
		free(env_path_current);
		env_path_current = malloc(strlen(path) + 1);
	}
		
	strcpy(env_path_current, path);
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
	printf("history\t display history of commands\n");
	printf("!<no>\t execute a specific historical command\n");
	printf("!!\t execute the last command\n");
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

/* Save the alias list to the ".aliases" file for alias persistence */
void save_aliases() {
	FILE *alias_file;
	const char *arg1 = "alias";
	
	// Set the working directory to HOME first
	if(chdir(env_home) == -1) {
		fprintf(stderr, "error: unable to reset working directory to home\n");
		return;
	}
	
	// Overwrite the old .aliases file
	alias_file = fopen(".aliases", "w");
	
	// Iterate through all alias's skipping any NULL entires
	for(int i = 0; i < ALIAS_MAX; i++) {
		char *arg2 = alias_key[i];
		char *arg3 = alias_value[i];
		
		if(arg2 == NULL || arg3 == NULL)
			// Record is empty, skip it
			continue;
		
		fprintf(alias_file, "%s %s %s\n", arg1, arg2, arg3);
	}
	
	fclose(alias_file);
	
	return;
}

/* Save the history for history persistence */
void save_history() {
	// Save the history in ascending order
	FILE *history_file;

	if(chdir(env_home) == -1) {
		fprintf(stderr, "error: unable to reset working directory to home\n");
		return;
	}

	history_file = fopen(".hist_list", "w");

	for(int i = 0; i < HISTORY_MAX; i++) {
		if(history_value[i].string == NULL)
			// Record is empty, skip it
			continue;

		fprintf(history_file, "%d %s\n",
			history_value[i].number, history_value[i].string);
	}

	fclose(history_file);

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
	argv - 	The array of argument strings (argv[0] is the program name).
			The last element in the array _must_ be NULL.
 */
void execute_process(char *argv[]) {
	pid_t new_process;

	// fork() a new child process
	new_process = fork();
	
	if(new_process < 0) {
		// Error occurred
		perror("error: fork() failed");
	}
	else if(new_process == 0) {
		// Child process
		if(execvp(argv[0], argv) == -1) {
			// Something went wrong when trying to execute the command
			perror("error: execvp() failed");
			exit(1);
		}
		
		exit(0);
	}
	else {
		// Wait for child to complete
		wait(NULL);
	}
	
	return;
}

/* Parse the tokenized input and perform the relevant and appropriate operation(s)
   
   Params:
	token_count - The number of tokens in the array
	token_list - The array of token strings
 */
void parse_tokens(int token_count, char *token_list[]) {
	// Check if command is an alias
	char *alias = alias_get(token_list[0]);
	
	if(alias != NULL) {
		// Don't want to alter original contents, so copy over to buffer
		char *alias_buffer = malloc(strlen(alias) + 1);
		strcpy(alias_buffer, alias);
		
		// Split the alias up into tokens so we can parse it easily
		int count = 0;
		char *alias_args[TOKEN_MAX];
		
		alias_args[count] = strtok(alias_buffer, delim);
		
		while(alias_args[count] != NULL) {
			count++;
			alias_args[count] = strtok(NULL, delim);
		}

		// Check if there's more than one token, if so append the arguments
		if(token_count > 1) {
			for(int i = 1; i < token_count; i++) {
				// Append arguments
				alias_args[count] = token_list[i];
				count++;
			}

			// Set the last argument to NULL for argument parsing
			alias_args[count] = NULL;
		}
		
		// Execute command
		parse_tokens(count, alias_args);
		
		free(alias_buffer);
		return;
	}

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
			printf("usage: cd [dir]\n");
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
		if(history_count == 0)
			fprintf(stderr, "error: no history recorded\n");
		else
			command_history();
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
			printf("usage: alias [<command1> <command2>]\n");
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
		// An unsupported internal command was called, we must assume it's an 
		// external command
		execute_process(token_list);
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
		chdir(env_home);
	}
	
	// Get the users PATH variable and set the shell PATH to that
	if((env_path_master = getenv("PATH")) == NULL)
		printf("warning: PATH variable undefined\n");
	else {
		env_path_current = malloc(strlen(env_path_master) + 1);
		strcpy(env_path_current, env_path_master);
	}
	
	// Load aliases
	printf("Initialising aliases:\n");
	alias_init();
	printf("\tdone!\n");
	
	// Load history
	printf("Initialising history:\n");
	history_init();
	printf("\tdone!\n\n");
	
	// Start the shell
	while(1) {
		printf("$ ");
		
		if(fgets(buffer, BUFFER_SIZE, stdin) == NULL) {
			// stdin stream closed, can't continue, end loop
			perror("error: stdin stream closed");
			break;
		}
		
		// Copy the raw input over to full_command to preserve the original input
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
		
		if(token_count == 0)
			// Can't parse nothing...
			continue;
			
		// We're using a circular array, so when we hit the max go back to start
		int history_pos = (history_count + 1) % HISTORY_MAX;
		
		if(strcmp(full_command, "!!") == 0) {
			// Previous history invokation (i.e. !! was entered)
			if(history_count == 0)
				// There's no recorded history
				printf("There are no commands stored in history\n");
			else {
				// Parse the previous command
				char *prev;

				for(int i = 1; i < HISTORY_MAX; i++) {
					// Iterate through history until a "valid" command is found
					// This ensures when !! is in the history it follows through
					prev = history_value[history_pos - i].string;

					if(prev == NULL)
						// Skip if value is NULL
						continue;

					if(prev[0] != '!')
						// Found a command that isn't a history invokation, stop loop
						break;
				}

				// Make sure the command isn't NULL or a history invokation
				if((prev != NULL) && (prev[0] != '!')) {
					strcpy(buffer, prev);
					history_invoke = true;
				}
				else {
					printf("warning: no previous history\n");
				}
			}
		}
		else if(full_command[0] == '!' && !(full_command[1] == '!')) {
			if(history_count == 0)
				printf("There are no commands stored in history\n");
			else {
				// Possibly a number passed for history invokation
				char *history_str_number = full_command + 1;
				int history_number = atoi(history_str_number);
				
				if(history_number == 0 || (history_number > history_count))
					// Failed - either invalid number passed or 0
					fprintf(stderr, "error: invalid history number\n");
				else {
					// Got a number, so attempt to fetch the history
					char *fetch = history_value[history_number % HISTORY_MAX].string;

					while(strcmp(fetch, "!!") == 0) {
						// Previous history command historically invoked, so get previous
						// command
						if(history_number > 1) {
							history_number--;
							fetch = history_value[history_number % HISTORY_MAX].string;
						}
						else {
							fprintf(stderr, "error: can't find any valid historical commands\n");
						}
					}

					// Make sure fetch isn't NULL and is a "valid" command
					if((fetch != NULL) && (strcmp(fetch, "!!") != 0)) {
						strcpy(buffer, fetch);
						history_invoke = true;
					}
				}
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
