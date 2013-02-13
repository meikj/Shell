#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define BUFFER_SIZE	512
#define MAX_TOKENS	50
#define PATH_MAX	512

/* pwd internal command */
void command_pwd() {
	char current_directory[PATH_MAX];
	
	getcwd(current_directory, PATH_MAX);
	printf("%s\n", current_directory);
	
	return;
}

/* help internal command */
void command_help() {
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
		// TODO: cd
		printf("Not supported... yet\n");
	}
	else if(strncmp(token_list[0], "pwd", 3) == 0) {
		// pwd called
		command_pwd();
	}
	else if(strncmp(token_list[0], "getpath", 7) == 0) {
		// getpath called
		// TODO: getpath
		printf("Not supported... yet\n");
	}
	else if(strncmp(token_list[0], "setpath", 7) == 0) {
		// setpath called
		// TODO: setpath
		printf("Not supported... yet\n");
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
	
	// Store the path to HOME
	char *path_home;
	
	// Get the users HOME and set the current directory to that
	if((path_home = getenv("HOME")) == NULL)
		printf("warning: HOME variable undefined\n");
	else {
		printf("[DEBUG]: HOME = %s\n", path_home);
		chdir(path_home);
	}
	
	// Start the shell
	while(1) {
		printf("$ ");
		
		if(fgets(buffer, BUFFER_SIZE, stdin) == NULL) {
			// stdin stream closed, can't continue
			perror("error: stdin stream closed");
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

