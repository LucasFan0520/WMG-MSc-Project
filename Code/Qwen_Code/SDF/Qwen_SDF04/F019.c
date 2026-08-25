// F019.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct {
    char* account;
    int amount;
    char* memo;
} Transaction;

Transaction* trans = NULL;
size_t t_count = 0;
size_t t_cap = 0;

void cleanup() {
    for (size_t i = 0; i < t_count; i++) {
        free(trans[i].account);
        free(trans[i].memo);
    }
    free(trans);
}

void add_trans(const char* account, int amount, const char* memo) {
    if (t_count == t_cap) {
        size_t new_cap = t_cap == 0 ? 4 : t_cap * 2;
        Transaction* temp = realloc(trans, new_cap * sizeof(Transaction));
        if (!temp) exit(1);
        trans = temp;
        t_cap = new_cap;
    }
    trans[t_count].account = strdup(account);
    trans[t_count].amount = amount;
    trans[t_count].memo = strdup(memo);
    if (!trans[t_count].account || !trans[t_count].memo) exit(1);
    t_count++;
}

int main(int argc, char** argv) {
    if (argc < 2) return 1;
    
    FILE* f = fopen(argv[1], "r");
    if (!f) return 1;
    
    char* line = NULL;
    size_t len = 0;
    while (getline(&line, &len, f) != -1) {
        if (line[strlen(line) - 1] == '\n') line[strlen(line) - 1] = '\0';
        char* account = strtok(line, " ");
        char* amount_str = strtok(NULL, " ");
        char* memo = strtok(NULL, "");
        if (account && amount_str && memo) {
            while (*memo && isspace((unsigned char)*memo)) memo++;
            char* endptr;
            long amount = strtol(amount_str, &endptr, 10);
            if (*endptr == '\0') {
                add_trans(account, (int)amount, memo);
            }
        }
    }
    fclose(f);
    free(line);
    
    line = NULL;
    len = 0;
    while (getline(&line, &len, stdin) != -1) {
        if (line[strlen(line) - 1] == '\n') line[strlen(line) - 1] = '\0';
        if (strcmp(line, "END") == 0) break;
        
        char* ptr = line;
        while (*ptr && isspace((unsigned char)*ptr)) ptr++;
        if (!*ptr) continue;
        
        char* cmd = ptr;
        while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
        if (*ptr) {
            *ptr = '\0';
            ptr++;
        }
        
        if (strcmp(cmd, "ADD") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* account = ptr;
            while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
            if (*ptr) { *ptr = '\0'; ptr++; }
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* amount_str = ptr;
            while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
            if (*ptr) { *ptr = '\0'; ptr++; }
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* memo = ptr;
            
            char* endptr;
            long amount = strtol(amount_str, &endptr, 10);
            if (*endptr == '\0') {
                add_trans(account, (int)amount, memo);
            }
        } else if (strcmp(cmd, "REVERSE") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* account = ptr;
            while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
            if (*ptr) { *ptr = '\0'; ptr++; }
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* amount_str = ptr;
            
            char* endptr;
            long amount = strtol(amount_str, &endptr, 10);
            if (*endptr == '\0') {
                for (size_t i = 0; i < t_count; i++) {
                    if (strcmp(trans[i].account, account) == 0 && trans[i].amount == (int)amount) {
                        free(trans[i].account);
                        free(trans[i].memo);
                        for (size_t j = i; j < t_count - 1; j++) {
                            trans[j] = trans[j + 1];
                        }
                        t_count--;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "BALANCE") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* account = ptr;
            int balance = 0;
            for (size_t i = 0; i < t_count; i++) {
                if (strcmp(trans[i].account, account) == 0) {
                    balance += trans[i].amount;
                }
            }
            printf("%d\n", balance);
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < t_count; i++) {
                printf("%s %d %s\n", trans[i].account, trans[i].amount, trans[i].memo);
            }
        }
    }
    
    free(line);
    cleanup();
    return 0;
}