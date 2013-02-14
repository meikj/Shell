#include <stdio.h>
#include <string.h>

#include "alias.h"

// Stores all the aliases
alias_t alias_list[ALIAS_LIMIT];

// The number of aliases present in the shell
int alias_count = 0;

/* Search the alias list for the value key
   
   Params:
   	key - The alias name (e.g. dir)
   	
   Returns:
   	The corresponding value (i.e. what the alias resolves to)
 */
char *alias_get(const char *key) {
	for(int i = 0; i < alias_count; i++) {
		if(strncmp(alias_list[i].key, key, strlen(key)))
			// Key found
			return alias_list[i].key;
	}
	
	return NULL;
}

/* Add an alias
   
   Params:
   	key - The alias name (e.g. dir)
   	value - What the alias resolves to (e.g. ls -a)
 */
void alias_add(const char *key, const char *value) {
	alias_t *alias = malloc(sizeof(alias_t));
	
	alias_list[alias_count] = alias;
	alias_count++;
	
	return;
}

