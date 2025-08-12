#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#define MAX_TOKEN_LEN 100

char yytext[MAX_TOKEN_LEN];
int yyleng;
int yylineno = 1;

FILE *yyin;

enum TokenType {
    T_KEYWORD,
    T_IDENTIFIER,
    T_INT_CONST,
    T_OPERATOR,
    T_PUNCTUATION,
    T_UNKNOWN,
    T_EOF
};

// List of C keywords (Reserverd words)
const char *keywords[] = {
    "int", "float", "char", "if", "else", "while", "for", "return", "void"
};
int num_keywords = sizeof(keywords) / sizeof(keywords[0]);

// Function prototypes
int isKeyword(const char *str);
int yylex();
void printToken(int token);

int main() {
    yyin = fopen("input.c", "r");
    if (!yyin) {
        printf("Error: Cannot open input file.\n");
        return 1;
    }

    int token;
    while ((token = yylex()) != T_EOF) {
        printToken(token);
    }

    fclose(yyin);
    return 0;
}

// Check if a string is a keyword
int isKeyword(const char *str) {
    for (int i = 0; i < num_keywords; i++) {
        if (strcmp(str, keywords[i]) == 0)
            return 1;
    }
    return 0;
}

// Main lexer function: reads next token
int yylex() {
    int ch;
    while ((ch = fgetc(yyin)) != EOF) {
        // Skip whitespace
        if (isspace(ch)) {
            if (ch == '\n') yylineno++;
            continue;
        }

        // Identifier or keyword
        if (isalpha(ch) || ch == '_') {
            int i = 0;
            yytext[i++] = ch;
            while (isalnum(ch = fgetc(yyin)) || ch == '_') {
                yytext[i++] = ch;
            }
            yytext[i] = '\0';
            yyleng = i;
            ungetc(ch, yyin);

            if (isKeyword(yytext)) return T_KEYWORD;
            return T_IDENTIFIER;
        }

        // Integer constant
        if (isdigit(ch)) {
            int i = 0;
            yytext[i++] = ch;
            while (isdigit(ch = fgetc(yyin))) {
                yytext[i++] = ch;
            }
            yytext[i] = '\0';
            yyleng = i;
            ungetc(ch, yyin);
            return T_INT_CONST;
        }

        // Operators (check for 2-character operators first)
        if (strchr("=<>!&|+-*/%^", ch)) {
            int i = 0;
            yytext[i++] = ch;
            int next = fgetc(yyin);
            if ((ch == '=' && next == '=') ||
                (ch == '!' && next == '=') ||
                (ch == '<' && next == '=') ||
                (ch == '>' && next == '=') ||
                (ch == '&' && next == '&') ||
                (ch == '|' && next == '|') ||
                (ch == '+' && next == '+') ||
                (ch == '-' && next == '-')) {
                yytext[i++] = next;
            } else {
                ungetc(next, yyin);
            }
            yytext[i] = '\0';
            yyleng = i;
            return T_OPERATOR;
        }

        // Brackets
        if (ch == '(' || ch == ')' || ch == '{' || ch == '}') {
            yytext[0] = ch;
            yytext[1] = '\0';
            yyleng = 1;
            return T_PUNCTUATION;
        }

        // If we reach here, it's unknown
        yytext[0] = ch;
        yytext[1] = '\0';
        yyleng = 1;
        return T_UNKNOWN;
    }

    return T_EOF;
}

// Print token type and value
void printToken(int token) {
    switch (token) {
        case T_KEYWORD:
            printf("Line %d: Keyword\t\t%s\n", yylineno, yytext);
            break;
        case T_IDENTIFIER:
            printf("Line %d: Identifier\t\t%s\n", yylineno, yytext);
            break;
        case T_INT_CONST:
            printf("Line %d: Integer Constant\t%s\n", yylineno, yytext);
            break;
        case T_OPERATOR:
            printf("Line %d: Operator\t\t%s\n", yylineno, yytext);
            break;
        case T_PUNCTUATION:
            printf("Line %d: Bracket\t\t\t%s\n", yylineno, yytext);
            break;
        case T_UNKNOWN:
            printf("Line %d: Unknown Token\t\t%s\n", yylineno, yytext);
            break;
    }
}