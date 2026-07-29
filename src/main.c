#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "tokenizace.c"


int main(int argc, char* argv[]) {
    if (argc != 2) {
        puts("Incorrect usage, Usage: helium <file>\n");
    } else {
        FILE *file = fopen(argv[1], "r");
        if (file == NULL) {
            printf("Error opening file\n");
            fclose(file);
            return 1;
        }
        FILE *asmfile = fopen("o.asm", "w");
        if (asmfile == NULL){
            puts("failed opening assembly file");
            fclose(asmfile);
            return 1;
        }
        fprintf(asmfile, "section .text\n");
        printf("Starting token parsing loop...\n");
        FILE *tmp = fopen("tmp.tmp", "w");
        if (tmp == NULL) {
            perror("Error opening temporary file");
            return 1;
        }
        fputs("section .data\n", tmp);
        for (int i = 0; i < 64; i++) {
            Token t1 = next_token(file);
            Token t2 = next_token(file);
            Token t3 = next_token(file);


            if (t1.type == Exit && t2.type == Intydzr && t3.type == strednik) {
                fprintf(asmfile, "    mov rax, 60\n    mov rdi, %i\n    syscall\n", t2.value);
            } else if (t1.type == funkce && t2.type == string && t3.type == strednik) {
                if (strcmp(t2.strvalue, "main") == 0) {
                    fputs("global _start\n_start:\n", asmfile);
                } else {
                    fprintf(asmfile, "%s:\n", t2.strvalue);
                }
            } else if (t1.type == zavolat && t2.type == string && t3.type == strednik) {
                fprintf(asmfile, "    jmp %s\n", t2.strvalue);
            } else if (t1.type == definuj && t2.type == string && t3.type == rovnitko) {
                Token t4 = next_token(file);
                Token t5 = next_token(file);
                Token t6 = next_token(file);
                if (t4.type == t_int && t5.type == Intydzr && t6.type == strednik) {
                    fprintf(asmfile, "    %s equ %i\n", t2.strvalue, t5.value);
                } else if (t4.type == t_str && t5.type == string && t6.type == strednik){
                    fprintf(tmp, "    %s db '%s'\n", t2.strvalue, t5.strvalue);
                    fprintf(tmp, "    %slen equ $-%s\n", t2.strvalue, t2.strvalue);
                }
            } else if (t1.type == Exit && t2.type == string && t3.type == strednik) {
                fprintf(asmfile, "    mov rax, 60\n    mov rdi, %s\n    syscall\n", t2.strvalue);
            } else if (t1.type == vyblej && t2.type == string && t3.type == strednik) {
                fprintf(asmfile, "    mov rax, 1\n    mov rdi, 1\n    mov rsi, %s\n    mov rdx, %slen\n    syscall\n", t2.strvalue, t2.strvalue);
            } else if (t1.type == input && t2.type == string && t3.type == strednik) {
                fprintf(asmfile, "    mov rax, 0\n    mov rdi, 0\n    mov rsi, %s\n    mov rdx, 256\n    syscall\n    %slen equ $-%s\n", t2.strvalue, t2.strvalue, t2.strvalue);
            }

        }
        fclose(asmfile);
        fclose(tmp);
        FILE *finalasm = fopen("out.asm", "w");
        FILE *temp = fopen("tmp.tmp", "r");
        FILE *asmfileo = fopen("o.asm", "r");
        fseek(temp, 0, SEEK_END);
        long size = ftell(temp);
        rewind(temp);
        char *kapsa2 = malloc(size + 1);
        fread(kapsa2, 1, size, temp);
        kapsa2[size] = '\0';
        fseek(asmfileo, 0, SEEK_END);
        long size2 = ftell(asmfileo);
        rewind(asmfileo);
        char *kapsa3 = malloc(size2 + 1);
        kapsa3 [size2] = '\0';
        fread(kapsa3, 1, size2, asmfileo);
        fputs(kapsa2, finalasm);
        fputs(kapsa3, finalasm);
        fclose(finalasm);
        fclose(temp);
        fclose(asmfileo);
        system("nasm -f elf64 out.asm");
        system("ld out.o -o out");
        //system("rm out.o && rm o.asm && rm out.asm && rm tmp.tmp");
        fclose(file);
        return 0;
    }
}