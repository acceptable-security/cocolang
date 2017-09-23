#include <stdlib.h>
#include "lexer.h"

// Determine if a value from the lexer is whitespace or not
inline bool lexer_is_whitespace(char value) {
    return value == ' ' || value == '\t' || value == '\n' || value == '\r';
}

// Determeine if we are still in the code bounds
inline bool lexer_in_bounds(lexer_t* lexer) {
    return lexer->index < lexer->code_length;
}

// Get the current lexer value
inline char lexer_current(lexer_t* lexer) {
    return lexer->code[lexer->index];
}

// Get the value near the current lexer value
inline char lexer_offset(lexer_t* lexer, int offset) {
    return lexer->code[lexer->index + offset];
}

// Make sure the lexer has space for n more chars
inline bool lexer_has_space(lexer_t* lexer, unsigned int amount) {
    return (lexer->code_length - lexer->index) >= amount;
}

// Determine if something is a valid name char
inline bool lexer_is_name(unsigned char value, bool end) {
    return (value >= 'a' && value <= 'z') ||
           (value >= 'A' && value <= 'Z' ) ||
           (value >= 0xC0) || // UTF-8 chars
           (end && (value >= '0' && value <= '9'));
}

// Determine if something is a valid number
inline bool lexer_is_number(char value) {
    return (value >= '0' && value <= '9');
}

// Advance the lexer a single characters
inline char lexer_advance(lexer_t* lexer) {
    // Make sure we don't go outof bounds
    if ( !lexer_in_bounds(lexer) ) {
        return 0;
    }

    // Get the character
    char current = lexer_current(lexer);

    // Handle newlines
    if ( current == '\n' ) {
        lexer->line_number++;
        lexer->line_offset = 0;
    }
    else {
        lexer->line_offset += 1;
    }

    lexer->index++;
    return current;
}

// Advance if we come into contact with something
inline bool lexer_advance_if(lexer_t* lexer, char value) {
    if ( lexer_current(lexer) == value ) {
        lexer_advance(lexer);
        return true;
    }
    else {
        return false;
    }
}

// Simple macro for wrapping lexer_advance_if
#define if_lexer_advance(L, V) if ( lexer_advance_if((L), (V)) )
#define if_not_lexer_advance(L, V) if ( !lexer_advance_if((L), (V)) )
