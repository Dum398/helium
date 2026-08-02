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
    t_time,
    vyblej,
    input,
    neg,
    dec,
    inc,
    sub,
    add,
    alloc,
    spat,
    isequal,
    foupn,
    frid,
    t_filehandle
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
    int negative = 0;
    if (kapsa[pos] == '-') {
        negative = 1;
        pos++;
    }
    if (isdigit(kapsa[pos])) {
        token.type = Intydzr;
        while (pos < len && isdigit(kapsa[pos])) {
            token.value = token.value * 10 + (kapsa[pos] - '0');
            pos++;
        }
        if (negative) {
            token.value = -token.value;
        }
        return token;
    }

    if (isalpha(kapsa[pos]) || kapsa[pos] == '/' || kapsa[pos] == '.') {
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
        } else if (strcmp(word, "print") == 0){
            token.type = vyblej;
        } else if (strcmp(word, "str") == 0) {
            token.type = t_str;
        } else if (strcmp(word, "input") == 0) {
            token.type = input;
        } else if (strcmp(word, "neg") == 0) {
            token.type = neg; 
        }else if (strcmp(word, "inc") == 0) {
            token.type = inc;
        } else if (strcmp(word, "dec") == 0) {
            token.type = dec;
        } else if (strcmp(word, "sub") == 0) {
            token.type = sub;
        } else if (strcmp(word, "add") == 0) {
            token.type = add;
        } else if (strcmp(word, "alloc") == 0) {
            token.type = alloc;
        } else if (strcmp(word, "sleep") == 0) {
            token.type = spat;
        } else if (strcmp(word, "time") == 0) {
            token.type = t_time;
        } else if (strcmp(word, "isequal") == 0) {
            token.type = isequal;
        } else if (strcmp(word, "fopen") == 0) {
            token.type = foupn;
        } else if (strcmp(word, "fread") == 0) {
            token.type = frid;
        } else if (strcmp(word, "filehandle") == 0) {
            token.type = t_filehandle;
        } else {
            token.type = string;
            strcpy(token.strvalue, word);
        }
        return token;
    }
    token.type = T_EOF;
    return token;
}