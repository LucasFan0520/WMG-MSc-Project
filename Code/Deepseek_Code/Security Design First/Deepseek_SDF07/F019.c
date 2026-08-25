// F019.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#define MAX_LINE 4096

typedef struct {
    char *account;
    long long amount;
    char *memo;
} Transaction;

typedef struct {
    Transaction *items;
    size_t count;
    size_t capacity;
} Journal;

static void trim_newline(char *s) {
    size_t len = strlen(s);
    if (len && s[len-1] == '\n') s[len-1] = '\0';
}

static char *safe_strdup(const char *s) {
    if (!s) return NULL;
    size_t len = strlen(s);
    char *p = malloc(len + 1);
    if (p) memcpy(p, s, len + 1);
    return p;
}

static void j_init(Journal *j) {
    j->items = NULL;
    j->count = 0;
    j->capacity = 0;
}

static void j_free(Journal *j) {
    for (size_t i = 0; i < j->count; i++) {
        free(j->items[i].account);
        free(j->items[i].memo);
    }
    free(j->items);
    j->items = NULL;
    j->count = 0;
    j->capacity = 0;
}

static long long j_balance(Journal *j, const char *account) {
    long long bal = 0;
    for (size_t i = 0; i < j->count; i++) {
        if (strcmp(j->items[i].account, account) == 0)
            bal += j->items[i].amount;
    }
    return bal;
}

static void j_add(Journal *j, const char *account, long long amount, const char *memo) {
    if (j->count == j->capacity) {
        size_t newcap = j->capacity ? j->capacity * 2 : 4;
        Transaction *tmp = realloc(j->items, newcap * sizeof(Transaction));
        if (!tmp) return;
        j->items = tmp;
        j->capacity = newcap;
    }
    j->items[j->count].account = safe_strdup(account);
    j->items[j->count].amount = amount;
    j->items[j->count].memo = safe_strdup(memo);
    if (j->items[j->count].account && j->items[j->count].memo)
        j->count++;
    else {
        free(j->items[j->count].account);
        free(j->items[j->count].memo);
    }
}

static void j_reverse(Journal *j, const char *account, long long amount) {
    j_add(j, account, -amount, "REVERSE");
}

static void j_balance_print(Journal *j, const char *account) {
    printf("%lld\n", j_balance(j, account));
}

static void j_report(Journal *j) {
    // unique accounts
    char **accs = NULL;
    size_t acnt = 0;
    for (size_t i = 0; i < j->count; i++) {
        int found = 0;
        for (size_t k = 0; k < acnt; k++) {
            if (strcmp(accs[k], j->items[i].account) == 0) { found = 1; break; }
        }
        if (!found) {
            char **tmp = realloc(accs, (acnt+1) * sizeof(char *));
            if (!tmp) { free(accs); return; }
            accs = tmp;
            accs[acnt] = safe_strdup(j->items[i].account);
            if (!accs[acnt]) { free(accs); return; }
            acnt++;
        }
    }
    for (size_t i = 0; i < acnt; i++) {
        printf("%s %lld\n", accs[i], j_balance(j, accs[i]));
        free(accs[i]);
    }
    free(accs);
}

static void load_file(Journal *j, const char *fname) {
    FILE *f = fopen(fname, "r");
    if (!f) return;
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), f)) {
        trim_newline(line);
        char account[256], memo[4096];
        long long amount;
        char *p = line;
        char *tok = strtok(p, " ");
        if (!tok) continue;
        strcpy(account, tok);
        tok = strtok(NULL, " ");
        if (!tok) continue;
        char *end;
        errno = 0;
        amount = strtoll(tok, &end, 10);
        if (errno != 0 || *end != '\0') continue;
        tok = strtok(NULL, "");
        if (tok) strcpy(memo, tok);
        else memo[0] = '\0';
        j_add(j, account, amount, memo);
    }
    fclose(f);
}

static void parse_cmd(const char *line, char *cmd, size_t cmdsz,
                      char *a1, size_t a1sz, char *a2, size_t a2sz, char *a3, size_t a3sz) {
    const char *p = line;
    while (isspace(*p)) p++;
    size_t i = 0;
    while (*p && !isspace(*p) && i < cmdsz - 1) cmd[i++] = *p++;
    cmd[i] = '\0';
    while (isspace(*p)) p++;
    i = 0;
    while (*p && !isspace(*p) && i < a1sz - 1) a1[i++] = *p++;
    a1[i] = '\0';
    while (isspace(*p)) p++;
    i = 0;
    while (*p && !isspace(*p) && i < a2sz - 1) a2[i++] = *p++;
    a2[i] = '\0';
    while (isspace(*p)) p++;
    i = 0;
    while (*p && i < a3sz - 1) a3[i++] = *p++;
    a3[i] = '\0';
}

int main(int argc, char **argv) {
    Journal j;
    j_init(&j);
    if (argc > 1) {
        load_file(&j, argv[1]);
    }
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        trim_newline(line);
        if (strcmp(line, "END") == 0) break;
        char cmd[32], a1[256], a2[64], a3[4096];
        parse_cmd(line, cmd, sizeof(cmd), a1, sizeof(a1), a2, sizeof(a2), a3, sizeof(a3));
        if (strcmp(cmd, "ADD") == 0) {
            long long amount = atoll(a2);
            j_add(&j, a1, amount, a3);
        } else if (strcmp(cmd, "REVERSE") == 0) {
            long long amount = atoll(a2);
            j_reverse(&j, a1, amount);
        } else if (strcmp(cmd, "BALANCE") == 0) {
            j_balance_print(&j, a1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            j_report(&j);
        }
    }
    j_free(&j);
    return 0;
}