// This file is for publicly available functions used by the parser.

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#include "lexer.h"

// Initialize a lexer
lexer_t* lexer_init(const char* special_tokens[], unsigned long special_count) {
    lexer_t* lexer = (lexer_t*) malloc(sizeof(lexer_t));

    if ( lexer == NULL ) {
        return NULL;
    }

    // Load some null values in first
    lexer->code = NULL;
    lexer->code_length = 0;
    lexer->index = 0;
    lexer->line_number = 0;
    lexer->line_offset = 0;

    lexer->special = special_tokens;
    lexer->special_count = special_count;

    return lexer;
}

// Preload the current and next token
static void lexer_preload_tokens(lexer_t* lexer) {
    assert(lexer != NULL);
    assert(lexer->code != NULL);

    lexer->previous.type = TOKEN_NULL;

    // Load the current token
    lexer_read_token(lexer, &lexer->current);

    if ( lexer->current.type == TOKEN_NULL ) {
        // TODO - print error
        return;
    }

    // Load the net token
    lexer_read_token(lexer, &lexer->next);
    // We don't need to error check since one valid token isn't technically
    // invalid at this stage.
}

// Let the lexer load the code into memory
void lexer_load_code(lexer_t* lexer, const char* path) {
    assert(lexer != NULL);

    // Open a file descriptor
    lexer->code_fd = open(path, O_RDONLY);

    // Get file length
    struct stat file_stat;
    fstat(lexer->code_fd, &file_stat);
    lexer->code_length = file_stat.st_size;

    // Map the file into memory
    lexer->code = mmap(NULL, lexer->code_length, PROT_READ, MAP_SHARED, lexer->code_fd, 0);

    // Preload the token
    lexer_preload_tokens(lexer);
}

// Get the current token's type
inline lexer_token_type_t lexer_current_type(lexer_t* lexer) {
    assert(lexer);
    return lexer->current.type;
}

// Get the next token's type
inline lexer_token_type_t lexer_next_type(lexer_t* lexer) {
    assert(lexer);
    return lexer->next.type;
}

// Free any references or memory the lexer
void lexer_close(lexer_t* lexer) {
    assert(lexer != NULL);

    if ( lexer->code != NULL ) {
        // Release the code mapping & file descriptor
        munmap((void*) lexer->code, lexer->code_length);
        close(lexer->code_fd);
    }

    free(lexer);
}
