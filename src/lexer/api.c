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

    lexer->code = NULL;
    lexer->code_length = 0;
    lexer->index = 0;
    lexer->line_number = 0;
    lexer->line_offset = 0;

    lexer->special = special_tokens;
    lexer->special_count = special_count;

    return lexer;
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
