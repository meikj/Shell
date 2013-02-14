#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define BUFFER_SIZE	512
#define MAX_TOKENS	50
#define PATH_MAX	512

// Environment code
char *env_home = NULL;
char *env_path_master = NULL;
char *env_path_current = NULL;

/* Execute clean up code */
void cleanup() {
	// Reset the PATH variable
	setenv("PATH", env_path_master, 1);
	
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
	if(strlen(env_path_current) < strlen(path))
		// Not enough space in current path to store new path
		env_path_current = malloc(strlen(path) + 1);
		
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
	printf("cd\tchange current working directory\n");
	printf("getpath\tprint system path\n");
	printf("setpath\tset system path\n");
	printf("pwd\tprint current working directory\n");
	printf("help\tlist the available internal shell commands\n");
	printf("exit\texit the shell\n");
	
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
	if(strncmp(token_list[0], "cd", 2) == 0) {
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
	else if(strncmp(token_list[0], "pwd", 3) == 0) {
		// pwd called
		command_pwd();
	}
	else if(strncmp(token_list[0], "getpath", 7) == 0) {
		// getpath called
		command_getpath();
	}
	else if(strncmp(token_list[0], "setpath", 7) == 0) {
		// setpath called
		if(token_count != 2)
			// Needs to be in format setpath <path>
			printf("usage: setpath <path>\n");
		else
			// token_list[1] = <path>
			command_setpath(token_list[1]);
	}
	else if(strncmp(token_list[0], "history", 7) == 0) {
		// history called
		// TODO: history
		printf("Not supported... yet\n");
	}
	else if(strncmp(token_list[0], "!!", 2) == 0) {
		// !! called
		// TODO: !!
		printf("Not supported... yet\n");
	}
	else if(strncmp(token_list[0], "!", 1) == 0) {
		// !<no> called
		// TODO: !<no>
		printf("Not supported... yet\n");
	}
	else if(strncmp(token_list[0], "alias", 5) == 0) {
		// alias called
		// TODO: alias
		printf("Not supported... yet\n");
	}
	else if(strncmp(token_list[0], "unalias", 7) == 0) {
		// unalias called
		// TODO: unalias
		printf("Not supported... yet\n");
	}
	else if(strncmp(token_list[0], "exit", 4) == 0) {
		// exit command called
		cleanup();
		exit(0);
	}
	else if(strncmp(token_list[0], "help", 4) == 0) {
		// help command called
		command_help();
	}
	else {
		// An unsupported internal command was called, we must assume it's an 
		// external command
		execute_process(token_count, token_list);
	}
	
	return;
}

int main(int argc, char *argv[]) {
	// User input buffer
	char buffer[BUFFER_SIZE];
	
	// Store tokens here (according to specification 50 tokens is reasonable)
	char *token_list[MAX_TOKENS];
	int token_count = 0;
	
	// Get the users PATH variable and set the shell PATH to that
	if((env_path_master = getenv("PATH")) == NULL)
		printf("warning: PATH variable undefined\n");
	else {
		printf("[DEBUG]: PATH = %s\n", env_path_master);
		env_path_current = env_path_master;
	}
	
	// Get the users HOME directory and set the current directory to that
	if((env_home = getenv("HOME")) == NULL)
		printf("warning: HOME variable undefined\n");
	else {
		printf("[DEBUG]: HOME = %s\n", env_home);
		chdir(env_home);
	}
	
	// Start the shell
	while(1) {
		printf("$ ");
		
		if(fgets(buffer, BUFFER_SIZE, stdin) == NULL) {
			// stdin stream closed, can't continue
			perror("error: stdin stream closed");
			cleanup();
			exit(1);
		}
		
		// Tokenize the user input and store the tokens in an array for easy 
		// parsing
		token_list[token_count] = strtok(buffer, " ");
		
		while(token_list[token_count] != NULL) {
			// There's still more tokens to get
			token_count++;
			token_list[token_count] = strtok(NULL, " ");
		}
		
		// Remove the trailing new line from the last token
		token_list[token_count - 1][strlen(token_list[token_count -1]) -1] = '\0';
		
		for(int i = 0; i <= token_count; i++)
			printf("token_list[%d] = %s\n", i, token_list[i]);
			
		
		// Now parse the token(s) and reset the counter
		parse_tokens(token_count, token_list);
		token_count = 0;
	}
	
	return 0;
}

