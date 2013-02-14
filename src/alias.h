#ifndef ALIAS_H
	#define ALIAS_H
	
	#define ALIAS_MAX	50
	
	// Function declarations
	void alias_init();
	char *alias_get(const char *key);
	bool alias_add(const char *key, const char *value);
	bool aias_remove(const char *key);
	
#endif

