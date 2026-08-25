/* F019.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *account;
    long amount;
    char *memo;
} Transaction;

static Transaction *txns = NULL;
static int tcount = 0;
static int tcap = 0;

static int ensure_cap(void) {
    if (tcount >= tcap) {
        int newcap = tcap == 0 ? 8 : tcap * 2;
        Transaction *tmp = realloc(txns, sizeof(Transaction) * newcap);
        if (!tmp) return -1;
        txns = tmp;
        tcap = newcap;
    }
    return 0;
}

static char *dup_str(const char *s) {
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (d) memcpy(d, s, len + 1);
    return d;
}

static int parse_amount(const char *s, long *out) {
    char *end = NULL;
    long val = strtol(s, &end, 10);
    if (end == s || *end != '\0') return -1;
    *out = val;
    return 0;
}

static void load_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    char line[4096];
    while (fgets(line, sizeof(line), f)) {
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') line[len - 1] = '\0';
        if (len > 1 && line[len - 2] == '\r') line[len - 2] = '\0';
        char account[256] = {0}, amt_str[64] = {0};
        int off1 = 0, off2 = 0;
        if (sscanf(line, "%255s%n %63s%n", account, &off1, amt_str, &off2) != 2) continue;
        long amount = 0;
        if (parse_amount(amt_str, &amount) != 0) continue;
        const char *memo = line + off2;
        while (*memo == ' ') memo++;
        if (ensure_cap() != 0) continue;
        txns[tcount].account = dup_str(account);
        txns[tcount].amount = amount;
        txns[tcount].memo = dup_str(memo);
        if (!txns[tcount].account || !txns[tcount].memo) {
            free(txns[tcount].account);
            free(txns[tcount].memo);
            continue;
        }
        tcount++;
    }
    fclose(f);
}

int main(int argc, char *argv[]) {
    if (argc > 1) {
        load_file(argv[1]);
    }
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') line[len - 1] = '\0';
        if (strcmp(line, "END") == 0) break;

        char cmd[64] = {0};
        int offset = 0;
        if (sscanf(line, "%63s%n", cmd, &offset) != 1) continue;

        if (strcmp(cmd, "ADD") == 0) {
            char account[256] = {0}, amt_str[64] = {0};
            int off2 = 0, off3 = 0;
            if (sscanf(line + offset, " %255s%n %63s%n", account, &off2, amt_str, &off3) != 2) continue;
            long amount = 0;
            if (parse_amount(amt_str, &amount) != 0) continue;
            const char *memo = line + offset + off3;
            while (*memo == ' ') memo++;
            if (ensure_cap() != 0) continue;
            txns[tcount].account = dup_str(account);
            txns[tcount].amount = amount;
            txns[tcount].memo = dup_str(memo);
            if (!txns[tcount].account || !txns[tcount].memo) {
                free(txns[tcount].account);
                free(txns[tcount].memo);
                continue;
            }
            tcount++;
        } else if (strcmp(cmd, "REVERSE") == 0) {
            char account[256] = {0}, amt_str[64] = {0};
            if (sscanf(line + offset, " %255s %63s", account, amt_str) != 2) continue;
            long amount = 0;
            if (parse_amount(amt_str, &amount) != 0) continue;
            if (ensure_cap() != 0) continue;
            txns[tcount].account = dup_str(account);
            txns[tcount].amount = -amount;
            txns[tcount].memo = dup_str("reversal");
            if (!txns[tcount].account || !txns[tcount].memo) {
                free(txns[tcount].account);
                free(txns[tcount].memo);
                continue;
            }
            tcount++;
        } else if (strcmp(cmd, "BALANCE") == 0) {
            char account[256] = {0};
            if (sscanf(line + offset, " %255s", account) != 1) continue;
            long total = 0;
            for (int i = 0; i < tcount; i++) {
                if (strcmp(txns[i].account, account) == 0)
                    total += txns[i].amount;
            }
            printf("%ld\n", total);
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < tcount; i++) {
                printf("%s %ld %s\n", txns[i].account, txns[i].amount, txns[i].memo);
            }
        }
    }
    for (int i = 0; i < tcount; i++) {
        free(txns[i].account);
        free(txns[i].memo);
    }
    free(txns);
    return 0;
}