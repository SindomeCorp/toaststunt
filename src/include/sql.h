#ifdef SQL_FOUND

#define SQL_SOFT_MAX_CONNECTIONS    5   // Soft cap; extra connections get closed after use.

#define SQL_PARSE_TYPES      2   // Return all strings if unset
#define SQL_PARSE_OBJECTS    4   // Turn "#100" into OBJ
#define SQL_SANITIZE_STRINGS 8   // Strip newlines from returned strings.

// Forward declarations used by sql.cc
extern int parse_float(const char *, double *);   // from numbers.cc

#endif // SQL_FOUND
