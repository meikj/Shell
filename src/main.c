#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Parse the tokenized input and perform the relevant and appropriate operation(s)
   
   Params:
	token_count - The number of tokens in the array
	token_list - The array of token strings
 */
void parse_tokens(int token_count, char *token_list[]) {
	// Use the first token as indication of what to do (e.g. exit, ls, etc.)
	if(strncmp(token_list[0], "exit", 4) == 0) {
		// exit command called
		exit(0);
	} else if(strncmp(token_list[0], "help", 4) == 0) {
		// help command called
		printf("help\tlist the available shell commands\n");
		printf("exit\texit the shell\n");
	} else {
		// An unsupported/invalid command was passed
		printf("Invalid command called. Please use 'help' for more information.\n");
	}
	return;
}

int main(int argc, char *argv[]) {
	// User input buffer
	char buffer[512];
	
	// Store tokens here (um, for the time being statically allocate 10 possible tokens)
	char *token_list[10];
	int token_count = 0;
	
	while(1) {
		printf("$ ");
		
		if(fgets(buffer, 512, stdin) == NULL) {
			// stdin stream closed, can't continue
			perror("stdin stream closed");
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
