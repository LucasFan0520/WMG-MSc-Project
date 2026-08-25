// F019.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 4096

static char *safe_strdup(const char *s) {
    if (!s) return NULL;
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (d) memcpy(d, s, len + 1);
    return d;
}

typedef struct { char *account; long balance; } Account;
static Account *arr = NULL;
static size_t cnt = 0, cap = 0;

static int grow(void) {
    if (cnt < cap) return 0;
    size_t nc = cap ? cap * 2 : 8;
    if (nc < cap) return -1;
    Account *t = realloc(arr, nc * sizeof(Account));
    if (!t) return -1;
    arr = t; cap = nc;
    return 0;
}

static int find_account(const char *account) {
    for (size_t i = 0; i < cnt; i++)
        if (strcmp(arr[i].account, account) == 0) return (int)i;
    return -1;
}

static void ensure_account(const char *account) {
    if (find_account(account) >= 0) return;
    if (grow() < 0) return;
    arr[cnt].account = safe_strdup(account);
    if (!arr[cnt].account) return;
    arr[cnt].balance = 0;
    cnt++;
}

static void load_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = '\0';
        char account[256] = {0};
        long amount = 0;
        char memo[256] = {0};
        if (sscanf(line, "%255s %ld %255s", account, &amount, memo) < 2) continue;
        ensure_account(account);
        int idx = find_account(account);
        if (idx >= 0) arr[idx].balance += amount;
    }
    fclose(f);
}

static void cmd_add(char *account, long amount, char *memo) {
    (void)memo;
    ensure_account(account);
    int idx = find_account(account);
    if (idx >= 0) arr[idx].balance += amount;
}

static void cmd_reverse(char *account, long amount) {
    int idx = find_account(account);
    if (idx < 0) return;
    arr[idx].balance -= amount;
}

static void cmd_balance(char *account) {
    int idx = find_account(account);
    if (idx < 0) { printf("0\n"); return; }
    printf("%ld\n", arr[idx].balance);
}

static void cmd_report(void) {
    for (size_t i = 0; i < cnt; i++)
        printf("%s %ld\n", arr[i].account, arr[i].balance);
}

static void cleanup(void) {
    for (size_t i = 0; i < cnt; i++) free(arr[i].account);
    free(arr); arr = NULL; cnt = 0; cap = 0;
}

int main(int argc, char *argv[]) {
    if (argc < 2) return 1;
    load_file(argv[1]);
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[64] = {0}; int n1 = 0;
        if (sscanf(line, "%63s%n", cmd, &n1) < 1) continue;
        char *rest = line + n1;
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "ADD") == 0) {
            char account[256] = {0}; long amount = 0; int n2 = 0, n3 = 0;
            if (sscanf(rest, "%255s%n %ld%n", account, &n2, &amount, &n3) < 2) continue;
            char *memo = rest + n3;
            while (*memo == ' ') memo++;
            cmd_add(account, amount, memo);
        } else if (strcmp(cmd, "REVERSE") == 0) {
            char account[256] = {0}; long amount = 0;
            if (sscanf(rest, "%255s %ld", account, &amount) < 2) continue;
            cmd_reverse(account, amount);
        } else if (strcmp(cmd, "BALANCE") == 0) {
            char account[256] = {0};
            if (sscanf(rest, "%255s", account) < 1) continue;
            cmd_balance(account);
        } else if (strcmp(cmd, "REPORT") == 0) {
            cmd_report();
        }
    }
    cleanup();
    return 0;
}