#include <stdio.h>
#include <string.h>

#include "alias.h"

// Stores all the aliases
alias_map_t alias_map;

/* Search the alias list for the value key
   
   Params:
   	key - The alias name (e.g. dir)
   	
   Returns:
   	The corresponding value (i.e. what the alias resolves to)
 */
char *alias_get(const char *key) {
	// TODO
	
	return NULL;
}

/* Add an alias
   
   Params:
   	key - The alias name (e.g. dir)
   	value - What the alias resolves to (e.g. ls -a)
 */
void alias_add(const char *key, const char *value) {
	// TODO
	
	return;
}

int main(void) {
	// Test
}
