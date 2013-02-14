#ifndef ALIAS_H
	#define ALIAS_H
	
	#define ALIAS_LIMIT	50
	
	// Represents an alias (key -> value)
	typedef struct {
		char *key; // alias name
		char *value; // what the alias resolves to
	} alias_t;
	
	char *alias_get(const char *key);
	void alias_add(const char *key, const char *value);
	
#endif

