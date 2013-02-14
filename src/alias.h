#ifndef ALIAS_H
	#define ALIAS_H
	
	#define ALIAS_LIMIT	50
	
	// Represents an alias (key -> value)
	typedef struct {
		char *key; // alias name
		char *value; // what the alias resolves to
	} alias_t;
	
	// Represents an alias map using alias_t
	typedef struct {
		int count;
		alias_t aliases[];
	} alias_map_t;
	
	// Function declarations
	char *alias_get(const char *key);
	void alias_add(const char *key, const char *value);
	void aias_remove(const char *key);
	
#endif

