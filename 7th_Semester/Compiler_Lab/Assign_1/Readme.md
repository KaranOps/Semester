Source : [https://www.ibm.com/docs/en/zos/3.1.0?topic=yacc-lex-output&utm_source=chatgpt.com]

lexeme (sequence of characters making up a token)

A string named "yytext" contains a sequence of characters making up a single input token. 

An integer variable named yyleng gives the number of characters in the yytext string.

Count \n to update yylineno

fgetc() returns the character read as an int

Token = fruit type (APPLE, BANANA, ORANGE) → category label
Pattern = description (APPLE → “round, red or green, stem on top”)
Lexeme = the actual fruit you picked up (“this green Granny Smith apple in my hand”)


