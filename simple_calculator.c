#include <math.h>
#include <float.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>



#define INVALID_IDX -1
#define MAX_RECURSION 5
int count_recursion = 0;
char* og_math_expr;
int og_math_expr_size;
char* rest_of_expr_ptr;
float result;

enum math_order {
    ADD_SUBTR = 1,
    MUL_DIV = 2,
    POWER = 3,
    PARENTHESIS = 4
};

int level = ADD_SUBTR;

typedef struct String_View {
    char* string;
    int   size;
} SV;



typedef union res {
    int     err_code;
    double* result;
} Res;



typedef struct {
    double result;
    char   next_operator;
    char*  rest_of_expr_ptr;
} Result;


void init_res(Res* res) {
    
    res->err_code = 0;
    res->result = NULL;
}



void print_math_expr(char* math_expression) {

    printf("\n\n-------------------------------------------\n\tMath expr: %s\n", math_expression);
    printf("\tMath expr len: %lu\n\n-----------------------------------------------------i\n\n", strlen(math_expression));
}


bool is_digit_or_dot(char c) {
    return c >= '0' && c <= '9' || c == '.';
}



float _get_first_operand(char* math_expression, int operand_position) {

    char* operand_position_ptr = math_expression + operand_position;
    int char_operand_size;
    char* beginning = math_expression;
    for (int i = operand_position - 1; i >= 0; i--) {
        if (!is_digit_or_dot(math_expression[i])) {
            beginning = operand_position_ptr - i + 1;
            break;
        }
    }

    char_operand_size = operand_position_ptr - beginning;
    printf("math_expression: %p\n", math_expression);
    printf("beginning: %p\n", beginning);
    printf("operand_position_ptr: %p\n", operand_position_ptr);

    char operand_str[char_operand_size + 1];
    strncpy(operand_str, beginning, char_operand_size);
    operand_str[char_operand_size] = '\0';

    printf("cx = %s\n", operand_str);

    return atof(operand_str);
}



Result _get_next_operand_and_operator(char* math_expression) {

    int char_operand_size;
    char* beginning = math_expression;
    int math_expr_size = strlen(math_expression);
    char* end = math_expression + math_expr_size;
    for (int i = 0; i < math_expr_size; i++) {
    //    printf("math_expression[%d]: %c\n", i, math_expression[i]);
        if (!is_digit_or_dot(math_expression[i])) {
            end = beginning + i;
            break;
        }
    }

    char_operand_size = end - beginning;
    // if (count_recursion == 1) {
    //     char operand_str[char_operand_size + 2];
    //     operand_str[0] = *(math_expression - 1);
    //     strncpy(operand_str + 1, beginning, char_operand_size);
    //     // printf("char_operand_size: %d\n", char_operand_size);
    //     operand_str[char_operand_size + 1] = '\0';

    //     char next_operator = *(end);

    //     return (Result) {
    //         .result = atof(operand_str),
    //         .rest_of_expr_ptr = end + 1,
    //         .next_operator = next_operator
    //     };
    // } else {
        char operand_str[char_operand_size + 1];
        strncpy(operand_str, beginning, char_operand_size);
        operand_str[char_operand_size] = '\0';

        char next_operator = *(end);

        return (Result) {
            .result = atof(operand_str),
            .rest_of_expr_ptr = end + 1,
            .next_operator = next_operator
        };
    //}
    // printf("\ncy = %s\n", operand_str);
}



bool char_is_operator(char c) {

    return  c == '+' ||
            c == '-' ||
            c == '*' || 
            c == '/' || 
            c == '%' || 
            c == '^';
}



bool starts_with_digit(char c) {

    return c >= '0' && c <= '9';
}




double init_result(int level) {

    if ( level == POWER || level == MUL_DIV)        return 1;
    else                                            return 0;
}



double _handle_math_operations(char* math_expression, int level) {
    count_recursion++;
    float current_result = init_result(level);
    printf("\n\n%d\tOne more _handle_math_operations round\n", count_recursion);
    printf("\n\n\tCurrent result Init: %f | current level: %d\n", current_result, level);

    printf("level: %d\n", level);
    print_math_expr(math_expression);

    // expression always starts with an operator. 
    // If the first thing is a digit, we are at the beginning of the expression and we assume the operator is +
    // if (starts_with_digit(next_operator)) next_operator = '+';
    Result res_op_1      = _get_next_operand_and_operator(math_expression);
    double x             = res_op_1.result;

    // check if the sign of the number was negative and adjust it
    if (math_expression[-1] == '-' && level == ADD_SUBTR) {
        x = -x;
    }

    char   next_operator = res_op_1.next_operator;

    // IMPORTANT: this sets the pointer to the right position in the original math expression string.
    rest_of_expr_ptr = math_expression;

    printf("current_result %f | operand: %f | current level: %d | next_operator %c\n", current_result, x,level, next_operator);

    if (next_operator == '*' && level <= MUL_DIV) {
        level = (level + 1) % 4;
        printf("level: %d", level);
        printf("Continue? (y/n) ");
        char ans = fgetc(stdin);
        fgetc(stdin); 

        if (ans != 'y') {
            printf("%c", ans);
            exit(0);
        }

        printf("Continuing\n");
        current_result = x * _handle_math_operations(res_op_1.rest_of_expr_ptr, level);
        print_math_expr(rest_of_expr_ptr);
        printf("\tcurrent result: %f\n", current_result);

        return current_result;
    }
    else if (next_operator == '+' && level <= ADD_SUBTR) {
        level = (level + 1) % 4;
        printf("result: %f\n", result);
        printf("current operand: %f\n", x); // 3
        printf("Continue? (y/n) ");
        char ans = fgetc(stdin);
        fgetc(stdin); 

        if (ans != 'y') {
            printf("%c", ans);
            exit(0);
        }

        printf("Continuing\n");
        current_result = x + _handle_math_operations(res_op_1.rest_of_expr_ptr, level);
        print_math_expr(rest_of_expr_ptr);
        printf("\tcurrent result: %f\n", current_result);

        return current_result;
    }
    else if (next_operator == '-' && level <= ADD_SUBTR && res_op_1.rest_of_expr_ptr[0] == '(') {
        level = (level + 1) % 4;
        printf("level: %d", level);
        printf("Continue? (y/n) ");
        char ans = fgetc(stdin);
        fgetc(stdin); 

        if (ans != 'y') {
            printf("%c", ans);
            exit(0);
        }

        printf("Continuing\n");
        current_result = x - _handle_math_operations(res_op_1.rest_of_expr_ptr, level);
        print_math_expr(rest_of_expr_ptr);
        printf("\tcurrent result: %f\n", current_result);

        return current_result;
    }

    rest_of_expr_ptr = res_op_1.rest_of_expr_ptr;
    
    printf("\n\tNo higher priority operation found in count_recursion: %d\n", count_recursion);

    current_result = x;

    level--;

    printf("current operand: %f | current result: %f", x, current_result);
    return current_result;
}



int main(int argc, char** argv) {
   
    if (argc != 2) {
        printf("\n\nERROR:\nNo more than one arg can be used. Exiting...\n\n");
        return EXIT_FAILURE;
    }

    printf("\n");
    for (int i = 1; i < argc; i++) {
        printf("->   %s\n", argv[i]);
    }

    char* math_expression = (char*) malloc(strlen(argv[1]) + 1);

    strncpy(math_expression, argv[1], strlen(argv[1]) + 1);

    printf("**\n-> %s\n", math_expression);

    result = 0;
    bool expr_starts_with_valid_sign =  math_expression[0] == '-' || 
                                        math_expression[0] == '+' || 
                                        starts_with_digit(math_expression[0]);

    if (!expr_starts_with_valid_sign) {
        printf("\n\nERROR: expression can start with digit/+/-\nExiting...\n\n");
        exit(0);
    }

    if (starts_with_digit(math_expression[0])) {
       char* temp = malloc(og_math_expr_size + 1);
       temp[0] = '+';
       
       strncpy(temp+1,math_expression, strlen(math_expression));
       
       // swap math_expression to point to temp and delete the original
       char* t = math_expression;
       math_expression = temp;
       
       free(t);
    }

    og_math_expr = math_expression;
    og_math_expr_size = strlen(math_expression);
    rest_of_expr_ptr = math_expression + 1; // we always start from the first operand

    printf("Entering operation hell\n");
    while (strlen(rest_of_expr_ptr) > 0) {

        //char sign = *(rest_of_expr_ptr - 1); // we look at one position before the first operand
        
        //if (sign == '+')
            result += _handle_math_operations(rest_of_expr_ptr, ADD_SUBTR);
        //else
        //    result -= _handle_math_operations(rest_of_expr_ptr, ADD_SUBTR);

        printf("** Result: %f\n", result);
        printf("** recursion_count: %d\n", count_recursion);
        count_recursion = 0;
        printf("\n\n--------------  again  ---------------\n\n");
    }
    printf("final result: %f\n", result);
    free(og_math_expr);

    return EXIT_SUCCESS;
}

