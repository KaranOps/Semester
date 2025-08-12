#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

// Token Definitions 
typedef enum {
    TOKEN_EOF = -1,
    // A. Operators
    // Arithmetic
    TOKEN_PLUS, TOKEN_MINUS, TOKEN_MUL, TOKEN_DIV, TOKEN_MOD, TOKEN_INC, TOKEN_DEC,
    // Relational
    TOKEN_LT, TOKEN_LE, TOKEN_GT, TOKEN_GE, TOKEN_EQ, TOKEN_NE,
    // Logical
    TOKEN_LOG_AND, TOKEN_LOG_OR, TOKEN_LOG_NOT,
    // Bitwise
    TOKEN_BIT_AND, TOKEN_BIT_OR, TOKEN_BIT_XOR, TOKEN_BIT_NOT, TOKEN_BIT_LSHIFT, TOKEN_BIT_RSHIFT,
    // Assignment
    TOKEN_ASSIGN, TOKEN_ADD_ASSIGN, TOKEN_SUB_ASSIGN, TOKEN_MUL_ASSIGN, TOKEN_DIV_ASSIGN, TOKEN_MOD_ASSIGN,

    // B. Reserved Words
    TOKEN_INT, TOKEN_FLOAT, TOKEN_CHAR, TOKEN_FOR, TOKEN_WHILE, TOKEN_IF, TOKEN_ELSE,

    // C & D. Identifier and Constants
    TOKEN_IDENTIFIER, TOKEN_INT_CONST,

    // E. Symbols
    TOKEN_LPAREN, TOKEN_RPAREN, TOKEN_LBRACE, TOKEN_RBRACE, TOKEN_SEMICOLON,

    TOKEN_INVALID
} TokenType;

// Global lexer variables
char lexeme[256];
int lexeme_len = 0;
int line_no = 1;
FILE *input_file;

// Reserved Keywords Table
typedef struct {
    const char *word;
    TokenType token;
} Keyword;

Keyword keyword_table[] = {
    {"int", TOKEN_INT},
    {"float", TOKEN_FLOAT},
    {"char", TOKEN_CHAR},
    {"for", TOKEN_FOR},
    {"while", TOKEN_WHILE},
    {"if", TOKEN_IF},
    {"else", TOKEN_ELSE},
    {NULL, TOKEN_EOF}
};

// Check if lexeme is a keyword
TokenType is_keyword(const char *str) {
    for (int i = 0; keyword_table[i].word != NULL; i++) {
        if (strcmp(str, keyword_table[i].word) == 0)
            return keyword_table[i].token;
    }
    return TOKEN_IDENTIFIER;
}

// Convert token enum to string
const char* token_name(TokenType token) {
    switch (token) {
        case TOKEN_INT: return "TOKEN_INT";
        case TOKEN_FLOAT: return "TOKEN_FLOAT";
        case TOKEN_CHAR: return "TOKEN_CHAR";
        case TOKEN_FOR: return "TOKEN_FOR";
        case TOKEN_WHILE: return "TOKEN_WHILE";
        case TOKEN_IF: return "TOKEN_IF";
        case TOKEN_ELSE: return "TOKEN_ELSE";
        case TOKEN_IDENTIFIER: return "TOKEN_IDENTIFIER";
        case TOKEN_INT_CONST: return "TOKEN_INT_CONST";
        case TOKEN_PLUS: return "TOKEN_PLUS";
        case TOKEN_MINUS: return "TOKEN_MINUS";
        case TOKEN_MUL: return "TOKEN_MUL";
        case TOKEN_DIV: return "TOKEN_DIV";
        case TOKEN_MOD: return "TOKEN_MOD";
        case TOKEN_INC: return "TOKEN_INC";
        case TOKEN_DEC: return "TOKEN_DEC";
        case TOKEN_LT: return "TOKEN_LT";
        case TOKEN_LE: return "TOKEN_LE";
        case TOKEN_GT: return "TOKEN_GT";
        case TOKEN_GE: return "TOKEN_GE";
        case TOKEN_EQ: return "TOKEN_EQ";
        case TOKEN_NE: return "TOKEN_NE";
        case TOKEN_LOG_AND: return "TOKEN_LOG_AND";
        case TOKEN_LOG_OR: return "TOKEN_LOG_OR";
        case TOKEN_LOG_NOT: return "TOKEN_LOG_NOT";
        case TOKEN_BIT_AND: return "TOKEN_BIT_AND";
        case TOKEN_BIT_OR: return "TOKEN_BIT_OR";
        case TOKEN_BIT_XOR: return "TOKEN_BIT_XOR";
        case TOKEN_ASSIGN: return "TOKEN_ASSIGN";
        case TOKEN_ADD_ASSIGN: return "TOKEN_ADD_ASSIGN";
        case TOKEN_SUB_ASSIGN: return "TOKEN_SUB_ASSIGN";
        case TOKEN_MUL_ASSIGN: return "TOKEN_MUL_ASSIGN";
        case TOKEN_DIV_ASSIGN: return "TOKEN_DIV_ASSIGN";
        case TOKEN_MOD_ASSIGN: return "TOKEN_MOD_ASSIGN";
        case TOKEN_LPAREN: return "TOKEN_LPAREN";
        case TOKEN_RPAREN: return "TOKEN_RPAREN";
        case TOKEN_LBRACE: return "TOKEN_LBRACE";
        case TOKEN_RBRACE: return "TOKEN_RBRACE";
        case TOKEN_SEMICOLON: return "TOKEN_SEMICOLON";
        case TOKEN_EOF: return "TOKEN_EOF";
        default: return "TOKEN_INVALID";
    }
}

// Lexical Analyzer Function
TokenType yylex() {
    int c, state = 0;
    lexeme_len = 0;
    lexeme[0] = '\0';

    while (1) {
        c = fgetc(input_file);
        lexeme[lexeme_len++] = c;
        lexeme[lexeme_len] = '\0';

        switch (state) {
            case 0:
                if (c == ' ' || c == '\t') { lexeme_len = 0; }
                else if (c == '\n') { line_no++; lexeme_len = 0; }
                else if (c == EOF) return TOKEN_EOF;
                else if (isalpha(c) || c == '_') { state = 1; }
                else if (isdigit(c)) { state = 2; }
                else if (c == '<') { state = 3; }
                else if (c == '>') { state = 4; }
                else if (c == '=') { state = 5; }
                else if (c == '!') { state = 6; }
                else if (c == '&') { state = 7; }
                else if (c == '|') { state = 8; }
                else if (c == '+') { state = 9; }
                else if (c == '-') { state = 10; }
                else if (c == '/') { state = 11; }
                else if (c == '*') return TOKEN_MUL;
                else if (c == '%') return TOKEN_MOD;
                else if (c == '^') return TOKEN_BIT_XOR;
                else if (c == '(') return TOKEN_LPAREN;
                else if (c == ')') return TOKEN_RPAREN;
                else if (c == '{') return TOKEN_LBRACE;
                else if (c == '}') return TOKEN_RBRACE;
                else if (c == ';') return TOKEN_SEMICOLON;
                else return TOKEN_INVALID;
                break;

            case 1:
                if (!(isalnum(c) || c == '_')) {
                    ungetc(c, input_file);
                    lexeme[--lexeme_len] = '\0';
                    return is_keyword(lexeme);
                }
                break;

            case 2:
                if (!isdigit(c)) {
                    ungetc(c, input_file);
                    lexeme[--lexeme_len] = '\0';
                    return TOKEN_INT_CONST;
                }
                break;

            case 3: // <
                if (c == '=') return TOKEN_LE;
                ungetc(c, input_file); lexeme[--lexeme_len] = '\0'; return TOKEN_LT;

            case 4: // >
                if (c == '=') return TOKEN_GE;
                ungetc(c, input_file); lexeme[--lexeme_len] = '\0'; return TOKEN_GT;

            case 5: // =
                if (c == '=') return TOKEN_EQ;
                ungetc(c, input_file); lexeme[--lexeme_len] = '\0'; return TOKEN_ASSIGN;

            case 6: // !
                if (c == '=') return TOKEN_NE;
                ungetc(c, input_file); lexeme[--lexeme_len] = '\0'; return TOKEN_LOG_NOT;

            case 7: // &
                if (c == '&') return TOKEN_LOG_AND;
                ungetc(c, input_file); lexeme[--lexeme_len] = '\0'; return TOKEN_BIT_AND;

            case 8: // |
                if (c == '|') return TOKEN_LOG_OR;
                ungetc(c, input_file); lexeme[--lexeme_len] = '\0'; return TOKEN_BIT_OR;

            case 9: // +
                if (c == '+') return TOKEN_INC;
                if (c == '=') return TOKEN_ADD_ASSIGN;
                ungetc(c, input_file); lexeme[--lexeme_len] = '\0'; return TOKEN_PLUS;

            case 10: // -
                if (c == '-') return TOKEN_DEC;
                if (c == '=') return TOKEN_SUB_ASSIGN;
                ungetc(c, input_file); lexeme[--lexeme_len] = '\0'; return TOKEN_MINUS;

            case 11: // /
                if (c == '/') { state = 12; lexeme_len = 0; }
                else if (c == '*') { state = 13; lexeme_len = 0; }
                else if (c == '=') return TOKEN_DIV_ASSIGN;
                else { ungetc(c, input_file); lexeme[--lexeme_len] = '\0'; return TOKEN_DIV; }
                break;

            case 12: // single-line comment
                if (c == '\n') { state = 0; lexeme_len = 0; line_no++; }
                break;

            case 13: // multi-line comment
                if (c == '*') state = 14;
                break;

            case 14:
                if (c == '/') { state = 0; lexeme_len = 0; }
                else if (c != '*') state = 13;
                break;
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    input_file = fopen(argv[1], "r");
    if (input_file == NULL) {
        perror("Cannot open file");
        return EXIT_FAILURE;
    }

    printf("Line\tToken\t\t\tLexeme\n");
    printf("----------------------------------------------\n");

    TokenType token;
    while ((token = yylex()) != TOKEN_EOF) {
        if (token != TOKEN_INVALID)
            printf("%d\t%-20s'%s'\n", line_no, token_name(token), lexeme);
        else
            printf("%d\t%-20s'%c'\n", line_no, token_name(token), lexeme[0]);
    }

    fclose(input_file);
    return 0;
}
