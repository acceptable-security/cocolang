// This file contains the internal code used by the lexer.

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"
#include "helper.h"

// Maximum size of a name
static const unsigned int lexer_max_name_size = 255;

// Advance the lexer past any whitespace
void lexer_skip_whitespace(lexer_t* lexer) {
    // Skip any whitespace
    while ( lexer_in_bounds(lexer) &&
            lexer_is_whitespace(lexer_advance(lexer)) );
}

// Advance the lexer past any comments
void lexer_skip_comments(lexer_t* lexer) {
    // Make sure we have space for a comment
    if ( !lexer_has_space(lexer, 2) ) {
        return;
    }

    if ( lexer_has_space(lexer, 2) &&
         lexer_current(lexer) == '/' && lexer_offset(lexer, 1) == '/') {
        // Advance pass the comment and anything until a newline
        lexer->index += 2;

        // Skip up to next new line or EOF
        while ( lexer_in_bounds(lexer) && lexer_current(lexer) != '\n' ) {
            lexer->index++;
        }

        // If we arrive at a new line, skip it.
        if ( lexer_in_bounds(lexer) ) {
            lexer->index++;

            // Handle the line properties
            lexer->line_number++;
            lexer->line_offset = 0;
        }
    }
}

// Attempt to read a name out of the lexer
bool lexer_attempt_name(lexer_t* lexer, lexer_token_t* dest) {
    if ( !lexer_is_name(lexer_current(lexer), false) ) {
        return false;
    }

    // We found a name. Allocate space for it
    // TODO - make a smaller allocation and grow that
    dest->type = TOKEN_NAME;
    dest->string_value = (char*) malloc(lexer_max_name_size * (lexer_max_name_size + 1));

    // Copy in the first character
    dest->string_value[0] = lexer_advance(lexer);
    dest->string_value[1] = 0; // Null terminate

    unsigned int name_size = 1;

    // Get all the valid name characters we can get
    while ( lexer_in_bounds(lexer) &&
            lexer_is_name(lexer_current(lexer), true) &&
            name_size < lexer_max_name_size ) {
        // Get the next valid character and null terminate
        dest->string_value[name_size++] = lexer_advance(lexer);
        dest->string_value[name_size] = 0;
    }

    return true;
}

// Attempt to read a single character value out of a string (escaped by escape)
// into dest. Returns true if something was successfully read into dest and
// false if it either encountered the escape or if it went OOB
bool lexer_read_string(lexer_t* lexer, char escape, char* dest) {
    if_lexer_advance(lexer, '\\') {
        char raw_escape = lexer_advance(lexer);

        switch ( raw_escape ) {
            case 'b':
                *dest = '\b';
                break;

            case 'n':
                *dest = '\n';
                break;

            case 't':
                *dest = '\t';
                break;

            case 'x':
                // TODO - hex parse
                *dest = 'x';
                break;

            case '0':
                *dest = '\0';
                break;

            // Ran out of bounds
            case 0:
                return false;
        }

        return true;
    }

    // Return nothing on escape found
    if_lexer_advance(lexer, escape) {
        return false;
    }

    // Just advance
    *dest = lexer_advance(lexer);
    return true;
}

// Attempt to read a string or char out of the lexer
bool lexer_attempt_string(lexer_t* lexer, lexer_token_t* dest) {
    // Attempt to read a string
    if_lexer_advance(lexer, '"') {
        dest->string_value = (char*) malloc(sizeof(char) * 128);
        unsigned int string_index = 0;
        unsigned int string_size = 128;

        // Read the string while we haven't hit the escape character and haven't
        // gone over the code.
        while ( lexer_read_string(lexer, '"', &dest->string_value[string_index++]) ) {
            if ( string_index >= string_size ) {
                string_size += 128;

                // Grow the string size
                char* tmp = realloc(dest->string_value, sizeof(char) * string_size);

                // Handle failed reallocations
                if ( tmp == NULL ) {
                    // TODO - print error
                    free(dest->string_value);

                    dest->string_value = NULL;
                    dest->type = TOKEN_NULL;

                    // Don't continue parsing
                    return true;
                }

                dest->string_value = tmp;
            }

            // Always null terminate
            dest->string_value[string_index] = 0;
        }

        dest->type = TOKEN_STRING;
        return true;
    }

    // Attempt to read a character
    if_lexer_advance(lexer, '\'') {
        // If this returns false then something wasn't read into char_value
        // Might be OOB or an empty char (invalid).
        if ( !lexer_read_string(lexer, '\'', &dest->char_value) ) {
            // TODO - print error
            dest->type = TOKEN_NULL;

            // Don't continue parsing after an error
            return true;

        }

        // Read the closing '
        if_not_lexer_advance(lexer, '\'') {
            // TODO - print error
            dest->type = TOKEN_NULL;

            // Don't continue parsing after an error.
            return true;
        }

        return true;
    }

    return false;
}

// Attempt to read a number or float out of the lexer
bool lexer_attempt_number(lexer_t* lexer, lexer_token_t* dest) {
    bool is_negative = false;

    // Handle negative values
    if ( lexer_current(lexer) == '-' ) {
        is_negative = true;

        // If the next letter isn't a number then this isn't a number
        if ( !lexer_is_number(lexer_offset(lexer, 1)) ) {
            return false;
        }

        lexer_advance(lexer);
    }

    // Read the number
    while ( lexer_is_number(lexer_current(lexer)) ) {
        // Add the number into the corret place
        dest->number_value *= 10;
        dest->number_value += (lexer_current(lexer)) - '0';

        lexer_advance(lexer);
    }

    // We're reading a float
    if_lexer_advance(lexer, '.') {
        dest->type = TOKEN_FLOAT;
        dest->float_value = (float) dest->number_value;

        float decimal_place = 1;

        while ( lexer_is_number(lexer_current(lexer)) ) {
            // Get the current place as a float
            float value = (float) ((lexer_current(lexer)) - '0');

            // Move down one place
            decimal_place /= 10.0;

            // Add the number into the correct place.
            dest->float_value += value * (1.0 / decimal_place);

            lexer_advance(lexer);
        }
    }
    else {
        // Apply negations and derive type
        if ( is_negative ) {
            dest->number_value = -dest->number_value;
            dest->type = TOKEN_SNUMBER;
        }
        else {
            dest->type = TOKEN_NUMBER;
        }
    }

    return true;
}

// Attempt to read a special value out of the lexer
bool lexer_attempt_special(lexer_t* lexer, lexer_token_t* dest) {
    for ( unsigned long i = 0; i < lexer->special_count; i++ ) {
        const char* token = lexer->special[i];
        unsigned long token_length = strlen(token);

        // If we don't have enough space for the token, ignore it.
        if ( !lexer_has_space(lexer, token_length) ) {
            continue;
        }

        bool found = true;

        // Compare the value in the lexer with the token
        for ( unsigned long j = 0; j < token_length; j++ ) {
            if ( lexer_offset(lexer, j) != token[j] ) {
                found = false;
                break;
            }
        }

        if ( !found ) {
            continue;
        }

        // Report the special token
        dest->type = TOKEN_SPECIAL;
        dest->string_value = (char*) token;

        // Move the lexer forward
        for ( unsigned long j = 0; j < token_length; j++ ) {
            lexer_advance(lexer);
        }

        return true;
    }

    return false;
}

// Advance the lexer and store the result into dest
void lexer_read_token(lexer_t* lexer, lexer_token_t* dest) {
    assert(lexer != NULL);
    assert(lexer->code != NULL);
    assert(dest != NULL);

    lexer_skip_whitespace(lexer);
    lexer_skip_comments(lexer);

    // Make sure we're still in bounds
    if ( !lexer_in_bounds(lexer) ) {
        dest->type = TOKEN_NULL;
        return;
    }

    // Attempt to read a name
    if ( lexer_attempt_name(lexer, dest) ) {
        return;
    }

    // Attempt to read a string (or char)
    if ( lexer_attempt_string(lexer, dest) ) {
        return;
    }

    // Attempt to read a number (includes both number and float)
    if ( lexer_attempt_number(lexer, dest) ) {
        return;
    }

    // Attempt to read a special value out of the stream
    if ( lexer_attempt_special(lexer, dest) ) {
        return;
    }

    // TODO - print error message
    dest->type = TOKEN_NULL;
}
