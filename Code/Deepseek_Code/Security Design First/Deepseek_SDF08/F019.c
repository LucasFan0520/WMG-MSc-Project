// F019.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#define MAX_LINE 4096

typedef struct trans {
    char *account;
    int amount; // can be negative for reverse?
    char *memo;
    struct trans *next;
} Trans;

Trans *transactions = NULL;

void trim_newline(char *s) {
    size_t len = strlen(s);
    if (len > 0 && s[len-1] == '\n') s[len-1] = '\0';
}

void add_transaction(const char *account, int amount, const char *memo) {
    Trans *t = malloc(sizeof(Trans));
    if (!t) { fprintf(stderr, "Memory error\n"); exit(1); }
    t->account = strdup(account);
    t->memo = strdup(memo);
    if (!t->account || !t->memo) { fprintf(stderr, "Memory error\n"); exit(1); }
    t->amount = amount;
    t->next = transactions;
    transactions = t;
}

void reverse_transaction(const char *account, int amount) {
    // Add a negative transaction
    add_transaction(account, -amount, "REVERSE");
}

int balance_account(const char *account) {
    int bal = 0;
    Trans *t = transactions;
    while (t) {
        if (strcmp(t->account, account) == 0) bal += t->amount;
        t = t->next;
    }
    return bal;
}

void report_transactions(void) {
    Trans *t = transactions;
    while (t) {
        printf("%s %d ", t->account, t->amount);
        for (char *s = t->memo; *s; s++) {
            putchar(*s == ' ' ? '_' : *s);
        }
        putchar('\n');
        t = t->next;
    }
}

void free_all(void) {
    Trans *t = transactions;
    while (t) {
        Trans *next = t->next;
        free(t->account);
        free(t->memo);
        free(t);
        t = next;
    }
}

void load_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), f)) {
        trim_newline(line);
        char *p = line;
        // skip leading spaces
        while (isspace(*p)) p++;
        if (!*p) continue;
        char *account = p;
        while (*p && !isspace(*p)) p++;
        if (!*p) continue;
        *p = '\0';
        p++;
        while (isspace(*p)) p++;
        char *amount_str = p;
        while (*p && !isspace(*p)) p++;
        if (!*p) continue;
        *p = '\0';
        p++;
        while (isspace(*p)) p++;
        char *memo = p;
        // remove trailing spaces
        char *end = memo + strlen(memo) - 1;
        while (end > memo && isspace(*end)) { *end = '\0'; end--; }
        char *endptr;
        errno = 0;
        int amount = strtol(amount_str, &endptr, 10);
        if (errno != 0 || *endptr != '\0') continue; // invalid amount
        add_transaction(account, amount, memo);
    }
    fclose(f);
}

int main(int argc, char **argv) {
    if (argc > 1) {
        load_file(argv[1]);
    }
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        trim_newline(line);
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        if (sscanf(line, "%31s", cmd) < 1) continue;
        if (strcmp(cmd, "REPORT") == 0) { report_transactions(); continue; }
        char *rest = line + strlen(cmd);
        while (isspace(*rest)) rest++;
        if (strcmp(cmd, "ADD") == 0 || strcmp(cmd, "REVERSE") == 0) {
            char *account = rest;
            while (*rest && !isspace(*rest)) rest++;
            if (!*rest) continue;
            *rest = '\0';
            rest++;
            while (isspace(*rest)) rest++;
            char *amount_str = rest;
            while (*rest && !isspace(*rest)) rest++;
            if (!*rest) continue;
            *rest = '\0';
            rest++;
            while (isspace(*rest)) rest++;
            char *memo = rest;
            char *endptr;
            errno = 0;
            int amount = strtol(amount_str, &endptr, 10);
            if (errno != 0 || *endptr != '\0') continue;
            if (strcmp(cmd, "ADD") == 0) {
                add_transaction(account, amount, memo);
            } else {
                reverse_transaction(account, amount);
            }
        } else if (strcmp(cmd, "BALANCE") == 0) {
            int bal = balance_account(rest);
            printf("%d\n", bal);
        }
    }
    free_all();
    return 0;
}