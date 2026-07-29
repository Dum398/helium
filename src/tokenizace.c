#include <stdio.h>
#include <ctype.h>
#include <string.h>

typedef enum {
    Exit,
    Intydzr,
    Mezera,
    funkce,
    blank,
    T_EOF,
    strednik,
    string,
    zavolat,
    definuj,
    rovnitko,
    t_int,
    t_str,
    vyblej,
    input
} TokenType;

typedef struct {
    TokenType type;
    int value;
    char strvalue[2048];
} Token;


Token next_token(FILE *file) {
    static char kapsa[8192];
    static int pos = 0;
    static int len = 0;

    Token token = {blank, 0};

    // Skip whitespace
    while (1) {
        if (pos >= len) {
            len = fread(kapsa, 1, sizeof(kapsa), file);
            pos = 0;
            if (len == 0) {
                token.type = T_EOF;
                return token;
            }
        }
        if (!isspace(kapsa[pos])) {
            break;
        }
        pos++;
    }
    printf("DEBUG: About to check char at pos %d: '%c' (ASCII %d)\n", pos, kapsa[pos], (int)kapsa[pos]);
    if (kapsa[pos] == ';') {
        pos++;
        token.type = strednik;
        return token;
    }

    if (kapsa[pos] == '=') {
        pos++;
        token.type = rovnitko;
        return token;
    }

    if (isdigit(kapsa[pos])) {
        token.type = Intydzr;
        while (pos < len && isdigit(kapsa[pos])) {
            token.value = token.value * 10 + (kapsa[pos] - '0');
            pos++;
        }
        return token;
    }

    if (isalpha(kapsa[pos])) {
        char word[64];
        int i = 0;
        while (pos < len && !isspace(kapsa[pos]) && kapsa[pos] != ';' && kapsa[pos] != '=' && i < 63) {
            word[i++] = kapsa[pos++];
        }
   
        word[i] = '\0';
        if (strcmp(word, "exit") == 0) {
            token.type = Exit;
        } else if (strcmp(word, "fn") == 0) {
            token.type = funkce;
        } else if (strcmp(word, "call") == 0) {
            token.type = zavolat;
        } else if (strcmp(word, "def") == 0) {
            token.type = definuj;
        } else if (strcmp(word, "int") == 0) {
            token.type = t_int;
        } else if (strcmp(word, "vyblej") == 0){
            token.type = vyblej;
        } else if (strcmp(word, "str") == 0) {
            token.type = t_str;
        } else if (strcmp(word, "input") == 0) {
            token.type = input;
        } else {
            token.type = string;
            strcpy(token.strvalue, word);
        }
        return token;
    }
    token.type = T_EOF;
    return token;
}