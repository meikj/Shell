#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "alias.h"

// Stores the alias names
char *alias_key[ALIAS_MAX];

// Stores the corresponding alias values
char *alias_value[ALIAS_MAX];

/* Initialise the alias structures */
void alias_init() {
	// Set all values to NULL to start with
	for(int i = 0; i < ALIAS_MAX; i++) {
		alias_key[i] = NULL;
		alias_value[i] = NULL;
	}
	
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
	// Iterate through the aliases looking for a match
	for(int i = 0; i < ALIAS_MAX; i++) {
		if(alias_key[i] == NULL)
			// We don't want to compare a NULL value
			continue;
		else if(strcmp(alias_key[i], key) == 0)
			// Found a match, return its corresponding value
			return alias_value[i];
	}
	
	return NULL;
}

/* Add an alias
   
   Params:
   	key - The alias name (e.g. dir)
   	value - What the alias resolves to (e.g. ls -a)
   	
   Returns:
   	If the addition failed, false is returned. Otherwise true.
 */
bool alias_add(const char *key, const char *value) {
	// Iterate through the aliases looking for a free space
	for(int i = 0; i < ALIAS_MAX; i++) {
		if(alias_key[i] == NULL) {
			// Found a free space, insert it here
			alias_key[i] = malloc(strlen(key) + 1);
			strcpy(alias_key[i], key);
			alias_value[i] = malloc(strlen(value) + 1);
			strcpy(alias_value[i], value);
			
			return true;
		}
	}
	
	return false;
}

/* Remove an alias
   
   Params:
   	key - The alias name (e.g. dir)
   
   Returns:
   	If the removal failed, false is returned. Otherwise true.
 */
bool alias_remove(const char *key) {
	// Iterate through the aliases looking for a match
	for(int i = 0; i < ALIAS_MAX; i++) {
		if(alias_key[i] == NULL)
			// We don't want to compare a NULL value
			continue;
		else if(strcmp(alias_key[i], key) == 0) {
			alias_key[i] = NULL;
			return true;
		}
	}
	
	return false;
}

