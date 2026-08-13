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
