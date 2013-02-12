#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Parse the arguments/tokens from user input
   
   Params:
	argc - Argument count (i.e. number of tokens)
	argv - Pointer to an array containing input tokens
 */
void parse_args(int argc, char *argv[]) {
	// Use the first token as indication of what to do (e.g. exit, ls, etc.)
	if(strncmp(argv[0], "exit", 4) == 0) {
		// exit command called
		exit(0);
	} else {
		// An unsupported/invalid command was passed
		printf("Invalid command called. Please use 'help' for more information.\n");
	}
	return;
}

int main(int argc, char *argv[]) {
	char buffer[512];
	
	// Store tokens here (um, for the time being statically allocate 10 possible tokens)
	char *token_list[10];
	int token_count = 0;
	
	while(1) {
		printf("$ ");
		
		if(fgets(buffer, 512, stdin) == NULL) {
			// stdin stream closed
			perror("stdin stream closed");
			exit(1);
		}
		
		// Store token pointer in token_list
		token_list[token_count] = strtok(buffer, " ");
		
		while(token_list[token_count] != NULL) {
			printf("token_list[%d] = %s\n", token_count, token_list[token_count]);
			token_count++;
			token_list[token_count] = strtok(NULL, " ");
		}
		
		parse_args(token_count, token_list);
		token_count = 0;
	}
	
	return 0;
}
