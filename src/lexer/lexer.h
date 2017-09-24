#ifndef _LEXER_H
#define _LEXER_H

typedef enum {
    TOKEN_NULL,     // Nothing stored.
    TOKEN_NAME,     // Stored in string_value
    TOKEN_SPECIAL,  // Stored in string_value
    TOKEN_STRING,   // Stored in string_value
    TOKEN_NUMBER,   // Stored in number_value
    TOKEN_SNUMBER,  // Stored in number_value
    TOKEN_FLOAT,    // Stored in float_value
    TOKEN_CHAR      // Stored in char_value
} lexer_token_type;

// Structure for individual lexer tokens
typedef struct {
    // Type of the token
    lexer_token_type type;

    // Union for all the values
    union {
        unsigned long number_value;
        char* string_value;
        float float_value;
        char char_value;
    };
} lexer_token_t;

typedef struct {
    // Information about the code we're reading
    int code_fd;               // Code file descriptor
    const char* code;          // Raw code in memory
    unsigned long code_length; // Byte length of code in memory

    // Lexer state information
    unsigned long index;       // The index in the code
    unsigned long line_number; // Line we are parsing
    unsigned long line_offset; // Which char in the line we are parsing

    // Special token list
    const char* special[];       // List of special tokens
    unsigned long special_count; // Amount of special tokens in that list

    // Currently loaded tokens
    lexer_token_t previous;
    lexer_token_t current;
    lexer_token_t next;
} lexer_t;


lexer_t* lexer_init(const char* special_tokens[], unsigned long special_count);
void lexer_load_code(lexer_t* lexer, const char* path);
void lexer_read_token(lexer_t* lexer, lexer_token_t* dest);
void lexer_close(lexer_t* lexer);

#endif
