#include <stdint.h>
#include "../list/list.h"

// An enum for the prefix ops
typedef enum {
    // Infix operations
    EXP_INF_ADD, // Add
    EXP_INF_SUB, // Subtract
    EXP_INF_MUL, // Multiply
    EXP_INF_DIV, // Divide
    EXP_INF_TYP, // Typecasting

    // Prefix operations
    EXP_PRE_INC, // Prefix increment
    EXP_PRE_DEC, // Prefix decrement

    // Postfix operations
    EXP_POS_INC, // Postfix increment
    EXP_POS_DEC  // Postfix decrement
} expression_op_t;

typedef enum {
    EXP_TYPE_NUM, // A number
    EXP_TYPE_STR, // A string
    EXP_TYPE_CAL, // A call
    EXP_TYPE_TYP, // A type
    EXP_TYPE_INF, // An infix expression
    EXP_TYPE_POS, // A postfix expression
    EXP_TYPE_PRE, // A prefix expression
} expression_type_t;

// Object for storing any exprssion
typedef struct {
    void* data;             // Pointer to a struct determined by type
    expression_type_t type; // Type of expression
} expression_t;

// Object for storing any infix expression
typedef struct {
    expression_t* lhs;  // Left hand side. (NULL if prefix op)
    expression_t* rhs;  // Right hand side. (NULL if postfix op)
    expression_op_t op; // The op of expression at play.
} expression_infix_t;

// Object for storing any monofix expression
typedef struct {
    expression_t* exp;  // The expression thats either left or right
    expression_op_t op; // The op of the current expresion.
} expression_monofix_t;

// Object for storing a number expression
typedef struct {
    uint64_t number;
} expression_number_t;

// Object for storing a signed number expression
typedef struct {
    int64_t number;
} expression_snumber_t;

// Object for storing a floating number expression
typedef struct {
    float number;
} expression_float_t;

// Object for storing a call
typedef struct {
    char* name;
    list_t* arguments;
} expression_call_t;
