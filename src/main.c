#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
	char buffer[512];
	
	while(1) {
		printf("$ ");
		if(fgets(buffer, 512, stdin) == NULL){ 
		   perror ("Error, Standard input closed");
	       return 0;}
		strtok (buffer," ,.-");
	    printf ("%s\n", buffer);
	    if(strncmp (buffer,"Exit",4) ==0){
		  return 0;}
	}
	
	return 0;
}
