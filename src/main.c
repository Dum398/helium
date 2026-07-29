#include <iso646.h>
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
            perror("Error opening file\n");
            return 1;
        }
        FILE *textsec = fopen("textsec.textsec", "w");
        if (textsec == NULL){
            perror("failed opening assembly file");
            return 1;
        }
        FILE *bssec = fopen("bssec.bssec", "w");
        if (bssec == NULL) {
            perror("Error opening assembly file");
            return 1;
        }
        fprintf(textsec, "section .text\n");
        printf("Starting token parsing loop...\n");
        FILE *datasec = fopen("datasec.datasec", "w");
        if (datasec == NULL) {
            perror("Error opening datasecoorary file");
            return 1;
        }
        fputs("section .bss\n", bssec);
        fputs("section .data\n", datasec);
        for (int i = 0; i < 64; i++) {
            Token t1 = next_token(file);
            Token t2 = next_token(file);
            Token t3 = next_token(file);


            if (t1.type == Exit && t2.type == Intydzr && t3.type == strednik) {
                fprintf(textsec, "    mov rax, 60\n    mov rdi, %i\n    syscall\n", t2.value);
            } else if (t1.type == funkce && t2.type == string && t3.type == strednik) {
                if (strcmp(t2.strvalue, "main") == 0) {
                    fputs("global _start\n_start:\n", textsec);
                } else {
                    fprintf(textsec, "%s:\n", t2.strvalue);
                }
            } else if (t1.type == zavolat && t2.type == string && t3.type == strednik) {
                fprintf(textsec, "    jmp %s\n", t2.strvalue);
            } else if (t1.type == definuj && t2.type == string && t3.type == rovnitko) {
                Token t4 = next_token(file);
                Token t5 = next_token(file);
                Token t6 = next_token(file);
                if (t4.type == t_int && t5.type == Intydzr && t6.type == strednik) {
                    fprintf(textsec, "    %s equ %i\n", t2.strvalue, t5.value);
                } else if (t4.type == t_str && t5.type == string && t6.type == strednik){
                    fprintf(datasec, "    %s db '%s'\n", t2.strvalue, t5.strvalue);
                    fprintf(datasec, "    %slen equ $-%s\n", t2.strvalue, t2.strvalue);
                }
            } else if (t1.type == Exit && t2.type == string && t3.type == strednik) {
                fprintf(textsec, "    mov rax, 60\n    mov rdi, %s\n    syscall\n", t2.strvalue);
            } else if (t1.type == vyblej && t2.type == string && t3.type == strednik) {
                fprintf(textsec, "    mov rax, 1\n    mov rdi, 1\n    mov rsi, %s\n    mov rdx, %slen\n    syscall\n", t2.strvalue, t2.strvalue);
            } else if (t1.type == input && t2.type == string && t3.type == strednik) {
                fprintf(textsec, "    mov rax, 0\n    mov rdi, 0\n    mov rsi, %s\n    mov rdx, 256\n    syscall\n    %slen equ 256\n", t2.strvalue, t2.strvalue);
                fprintf(bssec, "    %s resb 256\n", t2.strvalue);
            }

        }
        fclose(textsec);
        fclose(datasec);
        fclose(bssec);
        FILE *finalasm = fopen("out.asm", "w");

        FILE *dataseco = fopen("datasec.datasec", "r");
        FILE *textseco = fopen("textsec.textsec", "r");
        FILE *bsseco   = fopen("bssec.bssec", "r");
        
        fseek(dataseco, 0, SEEK_END);
        long size = ftell(dataseco);
        rewind(dataseco);
        
        char *kapsa2 = malloc(size);
        size_t n2 = fread(kapsa2, 1, size, dataseco);
        
        fseek(textseco, 0, SEEK_END);
        long size2 = ftell(textseco);
        rewind(textseco);
        
        char *kapsa3 = malloc(size2);
        size_t n3 = fread(kapsa3, 1, size2, textseco);
        
        fseek(bsseco, 0, SEEK_END);
        long size3 = ftell(bsseco);
        rewind(bsseco);
        
        char *kapsa4 = malloc(size3);
        size_t n4 = fread(kapsa4, 1, size3, bsseco);
        
        if (n2 != (size_t)size || n3 != (size_t)size2 || n4 != (size_t)size3) {
            perror("fread failed");
            return 1;
        }
        
        fwrite(kapsa2, 1, n2, finalasm);
        fwrite(kapsa4, 1, n4, finalasm);
        fwrite(kapsa3, 1, n3, finalasm);
        
        fclose(finalasm);
        fclose(dataseco);
        fclose(textseco);
        fclose(bsseco);
        system("nasm -f elf64 out.asm");
        system("ld out.o -o out");
        system("rm out.o && rm textsec.textsec && rm bssec.bssec && rm out.asm && rm datasec.datasec");
        fclose(file);
        return 0;
    }
}