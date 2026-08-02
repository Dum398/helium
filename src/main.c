#include <iso646.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "tokenizace.c"

int debugstate = 0;

int cleanup() {
    int excode;
    puts("Cleaning up...");
    system("rm out.o && rm textsec.textsec && rm bssec.bssec && rm datasec.datasec");
    if (debugstate != 1) {
        int excode = system("rm out.asm");
    } 
    if (excode == 1) {
        return 0;
    } else {
        return 1;
    }
}


void end(int endmode) {
    if (endmode == 0) {
        perror("Expected ;");
        puts("Exiting now...");
        cleanup();
        exit(1);
    } else if (endmode == 1) {
        perror("Syntax error");
        puts("Exiting now...");
        cleanup();
        exit(1);
    } else if (endmode == 2) {
        perror("Wrong file open mode");
        puts("Exiting now...");
        cleanup();
        exit(1);
    }
}

void displayhelp(int shouldexit) {
    puts("Usage: helium <file> [-d -h]\n    Flags: -h        Displays this help page\n           -d       Enables debug mode(wont delete assembly file)\n           --debug       Enables debug mode(wont delete assembly file)\n           --help     Displays this help page");
    if (shouldexit == 1) {
        exit(0);
    } else {
        return;
    }
}

int main(int argc, char* argv[]) {
    char *filename = "out";
    if (!(argc == 2 || argc == 3 || argc == 4)) {
        puts("Incorrect usage, Usage: helium <file> [-d]");
    } else {
        if (argc == 4) {
            if ((strcmp(argv[2], "-o") == 0) || (strcmp(argv[3], "-o") == 0)) {
                if (strcmp(argv[2], "-o") == 0) {
                    filename = argv[3];
                } else if (strcmp(argv[3], "-o")) {
                    filename = argv[4];
                }
            }
            if ((strcmp(argv[2], "-d") == 0) || (strcmp(argv[2], "--debug") == 0) || (strcmp(argv[3], "-d") == 0) || (strcmp(argv[3], "--debug") == 0)) {
                debugstate = 1;
            } else if ((strcmp(argv[2], "-h") == 0) || (strcmp(argv[2], "--help") == 0) || (strcmp(argv[3], "-h") == 0) || (strcmp(argv[3], "--help") == 0)) {
                displayhelp(1);
            }
        } else if (argc == 3) {
            if ((strcmp(argv[2], "-d") == 0) || (strcmp(argv[2], "--debug") == 0)) {
                debugstate = 1;
            } else if ((strcmp(argv[2], "-h") == 0) || (strcmp(argv[2], "--help") == 0)) {
                displayhelp(1);
            }
        } else if (argc == 2) {
            if (strcmp(argv[1], "-h") == 0) {
                displayhelp(1);
            } else if (strcmp(argv[1], "--help") == 0) {
                displayhelp(1);
            }
        }
   
        FILE *file = fopen(argv[1], "r");
        if (file == NULL) {
            perror("Error opening file\n");
            return 1;
        }
        FILE *textsec = fopen("textsec.textsec", "w");
        if (textsec == NULL){
            perror("failed opening text section file");
            return 1;
        }
        FILE *bssec = fopen("bssec.bssec", "w");
        if (bssec == NULL) {
            perror("Error opening bss section file");
            return 1;
        }
        fprintf(textsec, "section .text\n");
        printf("Starting token parsing loop...\n");
        FILE *datasec = fopen("datasec.datasec", "w");
        if (datasec == NULL) {
            perror("Error opening datasection file");
            return 1;
        }
        fputs("section .bss\n    bytes_read: resq 1\n", bssec);
        fputs("default rel\nsection .data\n    argc: dq 0\n    argv1: dq 0\n    argv2: dq 0\n", datasec);
        for (int i = 0; i < 64; i++) {
            Token t1 = next_token(file);
            Token t2 = next_token(file);
            Token t3 = next_token(file);

            if (t1.type == Exit && t2.type == Intydzr ) {
                if(t3.type == strednik) {
                    fprintf(textsec, "    mov rax, 60\n    mov rdi, %i\n    syscall\n", t2.value);
                } else end(0);
            } else if (t1.type == funkce && t2.type == string) {
                if (t3.type == strednik) {
                    if (strcmp(t2.strvalue, "main") == 0) {
                        fputs("global _start\n_start:\n    mov rax, [rsp]\n    mov [rel argc], rax\n    mov rax, [rsp+16]\n    mov [argv1], rax\n    mov rax, [rsp+24]\n    mov [argv2], rax\n", textsec);
                    } else {
                        fprintf(textsec, "%s:\n", t2.strvalue);
                    }
                } else end(0);
            } else if (t1.type == zavolat && t2.type == string ) {
                if (t3.type == strednik) {
                    fprintf(textsec, "    jmp %s\n", t2.strvalue);
                } else end(0);
            } else if (t1.type == definuj && t2.type == string && t3.type == rovnitko) {
                Token t4 = next_token(file);
                Token t5 = next_token(file);
                Token t6 = next_token(file);
                if (t4.type == t_int && t5.type == Intydzr ) {
                    if (t6.type == strednik) {
                        fprintf(bssec, "    %s: resq 1\n", t2.strvalue);
                        fprintf(textsec, "    mov qword [%s], %i\n", t2.strvalue, t5.value);
                    } else end(0);
                } else if (t4.type == t_str && t5.type == string){
                    if (t6.type == strednik) {
                        fprintf(datasec, "    %s db '%s', 0\n", t2.strvalue, t5.strvalue);
                    } else end(0);
                } else if (t4.type == t_time && t5.type == Intydzr) {
                    if (t6.type == strednik) {
                        fprintf(datasec, "    %s: dq %i\n        dq 0\n", t2.strvalue, t5.value);
                    } else end(0);
                }
            } else if (t1.type == Exit && t2.type == string ) {
                if (t3.type == strednik) {
                    fprintf(textsec, "    mov rax, 60\n    mov rdi, qword [%s]\n    syscall\n", t2.strvalue);
                } else end(0);
            } else if (t1.type == vyblej && t2.type == string ) {
                if (t3.type == strednik) {
                    fprintf(textsec, "    mov rax, 1\n    mov rdi, 1\n    lea rsi, [rel %s]\n    mov rdx, [rel %slen]\n    syscall\n", t2.strvalue, t2.strvalue);
                } else end(0);
            } else if (t1.type == input && t2.type == string ) {
                if (t3.type == strednik) {
                    fprintf(textsec, "    mov rax, 0\n    mov rdi, 0\n    lea rsi, [rel %s]\n    mov rdx, [rel %slen]\n    syscall\n    mov [rel %slen], rax\n", t2.strvalue, t2.strvalue, t2.strvalue);
                    fprintf(bssec, "    %s: resb 256\n", t2.strvalue);
                } else end(0);
            } else if (t1.type == neg && t2.type == string ) {
                if (t3.type == strednik) {
                    fprintf(textsec, "    neg qword [%s]\n", t2.strvalue);
                } else end(0);
            } else if (t1.type == inc && t2.type == string ) {
                if (t3.type == strednik) {
                    fprintf(textsec, "    inc qword [%s]\n", t2.strvalue);
                } else end(0);
            } else if (t1.type == dec && t2.type == string) {
                if (t3.type == strednik) {
                    fprintf(textsec, "    dec qword [%s]\n", t2.strvalue);
                } else end(0);
            } else if (t1.type == sub && ((t2.type == string  && t3.type == string) || (t2.type == string  && t3.type == Intydzr) || (t2.type == Intydzr && t3.type == string)  || (t2.type == Intydzr && t3.type == Intydzr))) {
                Token t4 = next_token(file);
                if (t4.type == strednik) {
                    if (t2.type == string) {
                        if (t3.type == string) {
                            fprintf(textsec, "    sub [%s], [%s]\n", t2.strvalue, t3.strvalue);
                        } else if (t3.type == Intydzr) {
                            fprintf(textsec, "    sub [%s], %i\n", t2.strvalue, t3.value);
                        }
                    } else if (t2.type == Intydzr) {
                        if (t3.type == string) {
                            fprintf(textsec, "    sub %i, [%s]\n", t2.value, t3.strvalue);
                        } else if (t3.type == Intydzr) {
                            fprintf(textsec, "    sub %i, %i\n", t2.value, t3.value);
                        }
                    }
                } else end(0);
            } else if (t1.type == add && ((t2.type == string  && t3.type == string) || (t2.type == string  && t3.type == Intydzr) || (t2.type == Intydzr && t3.type == string)  || (t2.type == Intydzr && t3.type == Intydzr))) {
                Token t4 = next_token(file);
                if (t4.type == strednik) {
                    if (t2.type == string) {
                        if (t3.type == string) {
                            fprintf(textsec, "    add [%s], [%s]\n", t2.strvalue, t3.strvalue);
                        } else if (t3.type == Intydzr) {
                            fprintf(textsec, "    add [%s], %i\n", t2.strvalue, t3.value);
                        }
                    } else if (t2.type == Intydzr) {
                        if (t3.type == Intydzr) {
                            fprintf(textsec, "    add %i, %i\n", t2.value, t3.value);
                        } else if (t3.type == string) {
                            fprintf(textsec, "    add %i, [%s]\n", t2.value, t3.strvalue);
                        }
                    }
                } else end(0);
            } else if (t1.type == alloc && ((t2.type == Intydzr && t3.type == Intydzr) || (t2.type == Intydzr && t3.type == string) || (t2.type == string && t3.type == string) || (t2.type == string && t3.type == Intydzr))) {
                Token t4 = next_token(file);
                if (t4.type == strednik) {
                    if (t2.type == string) {
                        if (t3.type == string) {
                            fprintf(bssec, "    %s: resb %s\n", t2.strvalue, t3.strvalue);
                        } else if (t3.type == Intydzr) {
                            fprintf(bssec, "    %s: resb %i\n", t2.strvalue, t3.value);
                        }
                    } else if (t2.type == Intydzr) {
                        if (t3.type == Intydzr) {
                            fprintf(bssec, "    %i: resb %i\n", t2.value, t3.value);
                        } else if (t3.type == string) {
                            fprintf(bssec, "    %i: resb %s\n", t2.value, t3.strvalue);
                        }
                    }
                } else end(0);
            } else if (t1.type == spat && t2.type == string) {
                if (t3.type == strednik) {
                    fprintf(textsec, "    mov rax, 35\n    lea rdi, [rel %s]\n    mov rsi, 0\n    syscall\n", t2.strvalue);
                } else end(0);
            } else if (t1.type == isequal && ((t2.type == string || t2.type == Intydzr) && (t3.type == string || t3.type == Intydzr))) {
                    Token t4 = next_token(file);
                    Token t5 = next_token(file);
                    if (t5.type == strednik) {
                        if (t4.type == string) {
                            if (t2.type == string) {
                                if (t3.type == string) {
                                    fprintf(textsec,"    mov rax, qword [rel %s]\n    cmp rax, qword [rel %s]\n    je %s\n", t2.strvalue, t3.strvalue, t4.strvalue);
                                } else if (t3.type == Intydzr) {
                                    fprintf(textsec,"    mov rax, qword [rel %s]\n    cmp rax,    %i\n    je %s\n", t2.strvalue, t3.value, t4.strvalue);
                                }
                            } else if (t2.type == Intydzr) {
                                if (t3.type == Intydzr) {
                                    fprintf(textsec, "    mov rax, %i\n    cmp rax,	%i\n    je  %s\n", t2.value, t3.value, t4.strvalue);
                                } else if (t3.type == string) {
                                    fprintf(textsec, "    mov rax, %i\n    cmp rax, qword [rel %s]\n    je  %s\n", t2.value, t3.strvalue, t4.strvalue);
                                }                                
                            } else end(1);
                        } else end(1);
                    } else end(0);
                    
                } else if (t1.type == foupn && t2.type == string && t3.type == string) {
                    Token t4 = next_token(file);
                    Token t5 = next_token(file);
                    if (t5.type == strednik) {
                        if (strcmp(t3.strvalue, "read") == 0) {
                            fprintf(textsec, "    mov rax, 2\n    lea rdi, [rel %s]\n    mov rsi, 0\n    mov rdx, 0\n    syscall\n    mov [rel %s], rax\n", t2.strvalue, t4.strvalue);
                        } else if (strcmp(t3.strvalue, "write") == 0) {
                            fprintf(textsec, "    mov rax, 2\n    lea rdi, [rel %s]\n    mov rsi, 1\n    mov rdx, 644\n    syscall\n    mov [rel %s], rax\n", t2.strvalue, t4.strvalue);
                        } else if (strcmp(t3.strvalue, "readwrite") == 0) {
                            fprintf(textsec, "    mov rax, 2\n    lea rdi, [rel %s]\n    mov rsi, 2\n    mov rdx, 644\n    syscall\n    mov [rel %s], rax\n", t2.strvalue, t4.strvalue);
                        } else end(2);
                    } else end(1);
                } else if (t1.type == frid && t2.type == string && t3.type == string) {
                    Token t4 = next_token(file);
                    Token t5 = next_token(file);
                    if (t5.type == strednik) {
                        fprintf(textsec, "    mov rax, 0\n    mov rdi, [rel %s]\n    lea rsi, [rel %s]\n    mov rdx, %i\n    syscall\n", t2.strvalue, t3.strvalue, t4.value);
                    } else end(0);
                } else if (t1.type == vyblejdosouboru && t2.type == string && t3.type == string) {
                    Token t4 = next_token(file);
                    if (t4.type == strednik) {
                        fprintf(textsec,"    mov rax, 1\n    mov rdi, [rel %s]\n    lea rsi, [rel %s]\n    mov rdx, [rel %slen]\n    syscall\n", t2.strvalue, t3.strvalue, t3.strvalue);
                    }  else end(0);
                } else if (t1.type == klose && t2.type == string) {
                    if (t3.type == strednik) {
                        fprintf(textsec, "    mov rax, 3\n    mov rdi, [rel %s]\n    syscall\n", t2.strvalue);
                    } else end(0);
                } else if (t1.type == defb && t2.type == string && t3.type == rovnitko) {
                    Token t4 = next_token(file);
                    Token t5 = next_token(file);
                    if (t4.type == t_int) {
                        if (t5.type == strednik) {
                            fprintf(bssec, "    %s: resb 1\n", t2.strvalue);
                        } else end(0);
                    } else if (t4.type == t_str){
                        if (t5.type == strednik) {
                            fprintf(bssec, "    %s: resq 1\n", t2.strvalue);
                        } else end(0);
                    } else if (t4.type == t_time) {
                        if (t5.type == strednik) {
                            fprintf(datasec, "    %s: dq\n        dq\n", t2.strvalue);
                        } else end(0);
                    } else if (t4.type == t_filehandle) {
                        if (t5.type == strednik) {
                            fprintf(bssec, "    %s: resq 1\n", t2.strvalue);
                        } else end(0);
                    } else if (t4.type == t_quad) {
                        if (t5.type == strednik) {
                            fprintf(bssec, "    %s: resq 1\n", t2.strvalue);
                        } else end(0);
                    }
                } else if (t1.type == risequeal && ((t2.type == string && t3.type == string) || (t2.type == string && t3.type == Intydzr))) {
                    Token t4 = next_token(file);
                    Token t5 = next_token(file);
                    if (t5.type == strednik) {
                        if (t4.type == string) {
                            if (t3.type == string) {
                                fprintf(textsec, "    cmp %s, %s\n    je %s\n", t2.strvalue, t3.strvalue, t4.strvalue);
                            } else {
                                fprintf(textsec, "    cmp %s, %i\n    je %s\n", t2.strvalue, t3.value, t4.strvalue);
                            }
                        } else end(1);
                    }else end(0);
                } else if (t1.type == movprint && t2.type == string) {
                    if (t3.type == strednik) {
                        fprintf(textsec, "    mov rax, 1\n    mov rdi, 1\n    mov rsi, [rel %s]\n    mov rdx, [rel %slen]\n    syscall\n", t2.strvalue, t2.strvalue);
                    }
                }else if (t1.type == movopen && t2.type == string && t3.type == string) {
                    Token t4 = next_token(file);
                    Token t5 = next_token(file);
                    if (t5.type == strednik) {
                        if (strcmp(t3.strvalue, "read") == 0) {
                            fprintf(textsec, "    mov rax, 2\n    mov rdi, [rel %s]\n    mov rsi, 0\n    mov rdx, 0\n    syscall\n    mov [rel %s], rax\n", t2.strvalue, t4.strvalue);
                        } else if (strcmp(t3.strvalue, "write") == 0) {
                            fprintf(textsec, "    mov rax, 2\n    mov rdi, [rel %s]\n    mov rsi, 1\n    mov rdx, 644\n    syscall\n    mov [rel %s], rax\n", t2.strvalue, t4.strvalue);
                        } else if (strcmp(t3.strvalue, "readwrite") == 0) {
                            fprintf(textsec, "    mov rax, 2\n    mov rdi, [rel %s]\n    mov rsi, 2\n    mov rdx, 644\n    syscall\n    mov [rel %s], rax\n", t2.strvalue, t4.strvalue);
                        } else end(2);
                    } else end(1);
                } else if (t1.type == movread && t2.type == string && t3.type == string) {
                    Token t4 = next_token(file);
                    Token t5 = next_token(file);
                    if (t5.type == strednik) {
                        fprintf(textsec, "    mov rax, 0\n    mov rdi, [rel %s]\n    mov rsi, [rel %s]\n    mov rdx, %i\n    syscall\n", t2.strvalue, t3.strvalue, t4.value);
                    } else end(0);
                } else if (t1.type == movfprint && t2.type == string && t3.type == string) {
                    Token t4 = next_token(file);
                    if (t4.type == strednik) {
                        fprintf(textsec,"    mov rax, 1\n    mov rdi, [rel %s]\n    mov rsi, [rel %s]\n    mov rdx, [rel %slen]\n    syscall\n", t2.strvalue, t3.strvalue, t3.strvalue);
                    }  else end(0);
                } else if (t1.type == string && (t2.type == rovnitko && t3.type == string || t2.type == rovnitko && t3.type == Intydzr)) {
                    Token t4 = next_token(file);
                    if (t4.type == strednik) {
                        if (t3.type == string) {
                            fprintf(textsec, "    mov %s, [rel %s]", t1.strvalue, t3.strvalue);
                        } else if (t3.type == Intydzr) {
                            fprintf(textsec, "    mov %s, '%i'", t1.strvalue, t3.value);
                        }
                    }
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
        puts("Building...");
        system("nasm -f elf64 out.asm");
        char* ldcommand;
        asprintf(&ldcommand, "ld out.o -o %s", filename);
        system(ldcommand);
        int cleanupcode = cleanup();
        if (cleanupcode != 1) {
            perror("Cleanup Failed");
            exit(1);
        }
        fclose(file);
        return 0;
    }
}
