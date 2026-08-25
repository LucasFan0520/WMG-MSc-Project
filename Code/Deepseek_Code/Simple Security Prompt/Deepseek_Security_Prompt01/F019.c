// F019.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *account; int amount; char *memo; } Trans;
Trans *trans = NULL;
int trans_count = 0, trans_cap = 0;

void add_trans(const char *account, int amount, const char *memo) {
    if (trans_count == trans_cap) {
        trans_cap = trans_cap ? trans_cap * 2 : 4;
        trans = realloc(trans, sizeof(Trans) * trans_cap);
    }
    trans[trans_count].account = strdup(account);
    trans[trans_count].amount = amount;
    trans[trans_count].memo = strdup(memo);
    trans_count++;
}

void reverse_trans(const char *account, int amount) {
    add_trans(account, -amount, "REVERSE");
}

int balance_account(const char *account) {
    int bal = 0;
    for (int i = 0; i < trans_count; i++)
        if (strcmp(trans[i].account, account) == 0)
            bal += trans[i].amount;
    return bal;
}

void report_journal(void) {
    for (int i = 0; i < trans_count; i++)
        printf("%s %d %s\n", trans[i].account, trans[i].amount, trans[i].memo);
}

void load_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    char line[4096];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = '\0';
        char account[256], memo[4096];
        int amount;
        if (sscanf(line, "%255s %d %4095[^\n]", account, &amount, memo) == 3)
            add_trans(account, amount, memo);
    }
    fclose(f);
}

int main(int argc, char **argv) {
    if (argc > 1) load_file(argv[1]);
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32], account[256], memo[4096];
        int amount;
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%*s %255s %d %4095[^\n]", account, &amount, memo) == 3)
                add_trans(account, amount, memo);
        } else if (strcmp(cmd, "REVERSE") == 0) {
            if (sscanf(line, "%*s %255s %d", account, &amount) == 2)
                reverse_trans(account, amount);
        } else if (strcmp(cmd, "BALANCE") == 0) {
            if (sscanf(line, "%*s %255s", account) == 1)
                printf("%d\n", balance_account(account));
        } else if (strcmp(cmd, "REPORT") == 0) report_journal();
    }
    for (int i = 0; i < trans_count; i++) { free(trans[i].account); free(trans[i].memo); }
    free(trans);
    return 0;
}