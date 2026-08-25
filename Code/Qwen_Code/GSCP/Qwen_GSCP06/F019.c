// F019.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 4096

char* get_word(char** str) {
    while (**str == ' ') (*str)++;
    if (!**str) return NULL;
    char* start = *str;
    while (**str && **str != ' ') (*str)++;
    if (**str) {
        **str = '\0';
        (*str)++;
    }
    return start;
}

typedef struct Trans {
    char* account;
    int amount;
    char* memo;
    struct Trans* next;
} Trans;

Trans* head = NULL;

int get_balance(const char* account) {
    int bal = 0;
    for (Trans* t = head; t; t = t->next) {
        if (strcmp(t->account, account) == 0) {
            bal += t->amount;
        }
    }
    return bal;
}

int main(int argc, char* argv[]) {
    if (argc > 1) {
        FILE* f = fopen(argv[1], "r");
        if (f) {
            char l[MAX_LINE];
            while (fgets(l, sizeof(l), f)) {
                l[strcspn(l, "\n")] = 0;
                char* p = l;
                char* acc = get_word(&p);
                char* amt_str = get_word(&p);
                int amt = atoi(amt_str);
                if (amt != 0 || strcmp(amt_str, "0") == 0) {
                    Trans* n = malloc(sizeof(Trans));
                    n->account = strdup(acc);
                    n->amount = amt;
                    n->memo = strdup(p);
                    n->next = head;
                    head = n;
                }
            }
            fclose(f);
        }
    }
    
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char* p = line;
        char* cmd = get_word(&p);
        if (!cmd) continue;
        
        if (strcmp(cmd, "ADD") == 0) {
            char* acc = get_word(&p);
            char* amt_str = get_word(&p);
            int amt = atoi(amt_str);
            Trans* n = malloc(sizeof(Trans));
            n->account = strdup(acc);
            n->amount = amt;
            n->memo = strdup(p);
            n->next = head;
            head = n;
        } else if (strcmp(cmd, "REVERSE") == 0) {
            char* acc = get_word(&p);
            int amt = atoi(p);
            Trans* n = malloc(sizeof(Trans));
            n->account = strdup(acc);
            n->amount = -amt;
            n->memo = strdup("REVERSE");
            n->next = head;
            head = n;
        } else if (strcmp(cmd, "BALANCE") == 0) {
            char* acc = get_word(&p);
            printf("%d\n", get_balance(acc));
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (Trans* t = head; t; t = t->next) {
                printf("%s %d %s\n", t->account, t->amount, t->memo);
            }
        }
    }
    return 0;
}