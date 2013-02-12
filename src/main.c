#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
	char buffer[512];
	char *token;
	
	while(1) {
		printf("$ ");
		
		if(fgets(buffer, 512, stdin) == NULL) {
			// stdin stream closed
			perror("Error: stdin stream closed");
			exit(1);
		}
		
		token = strtok(buffer, " ");
		
		while(token != NULL) {
			printf("token = %s\n", token);
			token = strtok(NULL, " ");
		}
		
		if(strncmp(buffer, "exit", 4) == 0) {
			// exit command called, quit program
			return 0;
		}
	}
	
	return 0;
}
