#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

/* Execute an external process using the arguments provided
   
   Params:
	argc - The number of arguments
	argv - The array of argument strings (argv[0] is the program name)
	
   Returns:
	0 - If successful
	1 - If fork() failed
	2 - If exec() failed
 */
int execute_process(int argc, char *argv[]) {
	pid_t new_process;
	
	// fork() a new child process
	new_process = fork();
	
	if(new_process < 0) {
		// Error occurred
		return 1;
	}
	else if(new_process == 0) {
		// Child process
		printf("[DEBUG]: execute_process(): execlp() called\n");
		
		if(execlp(argv[0], argv) == -1) {
			// An error occured
			return 2;
		}
	}
	else {
		// Wait for child to complete
		wait(NULL);
		printf("[DEBUG]: execute_process(): child process complete\n");
	}
	
	return 0;
}

/* Parse the tokenized input and perform the relevant and appropriate operation(s)
   
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
		// TODO: pwd
		printf("Not supported... yet\n");
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
		printf("help\tlist the available internal shell commands\n");
		printf("exit\texit the shell\n");
	}
	else {
		// An unsupported internal command was called, we must assume it's an external command
		
		// TODO: Implement execute_process() code
		
		printf("'%s' is an unrecognised internal command, please use 'help' for a list of available commands\n", token_list[0]);
	}
	return;
}

int main(int argc, char *argv[]) {
	// User input buffer
	char buffer[512];
	
	// Store tokens here (according to specification 50 tokens is reasonable)
	char *token_list[50];
	int token_count = 0;
	
	while(1) {
		printf("$ ");
		
		if(fgets(buffer, 512, stdin) == NULL) {
			// stdin stream closed, can't continue
			perror("error: stdin stream closed");
			exit(1);
		}
		
		// Tokenize the user input and store the tokens in an array for easy parsing
		token_list[token_count] = strtok(buffer, " ");
		
		while(token_list[token_count] != NULL) {
			// There's still more tokens to get
			printf("token_list[%d] = %s\n", token_count, token_list[token_count]);
			token_count++;
			token_list[token_count] = strtok(NULL, " ");
		}
		
		// Now parse the token(s) and reset the counter
		parse_tokens(token_count, token_list);
		token_count = 0;
	}
	
	return 0;
}
