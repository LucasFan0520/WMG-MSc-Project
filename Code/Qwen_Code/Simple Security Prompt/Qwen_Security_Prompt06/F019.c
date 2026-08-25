// F019.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line() {
    size_t cap = 256, len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    while (1) {
        if (!fgets(buf + len, cap - len, stdin)) {
            if (len == 0) { free(buf); return NULL; }
            break;
        }
        len = strlen(buf);
        if (len > 0 && buf[len-1] == '\n') { buf[len-1] = '\0'; break; }
        if (len == cap - 1) {
            cap *= 2;
            char *tmp = realloc(buf, cap);
            if (!tmp) { free(buf); return NULL; }
            buf = tmp;
        }
    }
    return buf;
}

typedef struct Account {
    char *name;
    int balance;
    struct Account *next;
} Account;

Account *accounts = NULL;

Account *find_account(const char *name) {
    for (Account *a = accounts; a; a = a->next) {
        if (strcmp(a->name, name) == 0) return a;
    }
    return NULL;
}

void add_account(const char *name, int amount) {
    Account *a = find_account(name);
    if (!a) {
        a = malloc(sizeof(Account));
        a->name = strdup(name);
        a->balance = 0;
        a->next = accounts;
        accounts = a;
    }
    a->balance += amount;
}

void load_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    char *line;
    while ((line = read_line())) {
        char name[256] = {0};
        int amount = 0;
        if (sscanf(line, "%255s %d", name, &amount) == 2) {
            add_account(name, amount);
        }
        free(line);
    }
    fclose(f);
}

void print_balance(const char *name) {
    Account *a = find_account(name);
    printf("%d\n", a ? a->balance : 0);
}

void report_accounts() {
    for (Account *a = accounts; a; a = a->next) {
        printf("%s %d\n", a->name, a->balance);
    }
}

int main(int argc, char *argv[]) {
    if (argc > 1) {
        load_file(argv[1]);
    }
    
    char *line;
    while ((line = read_line())) {
        if (strcmp(line, "END") == 0) break;
        char cmd[16] = {0}, name[256] = {0};
        int amount = 0;
        sscanf(line, "%15s %255s %d", cmd, name, &amount);
        
        if (strcmp(cmd, "ADD") == 0) {
            add_account(name, amount);
        } else if (strcmp(cmd, "REVERSE") == 0) {
            Account *a = find_account(name);
            if (a) a->balance -= amount;
        } else if (strcmp(cmd, "BALANCE") == 0) {
            print_balance(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_accounts();
        }
        free(line);
    }
    
    Account *a = accounts;
    while (a) {
        Account *next = a->next;
        free(a->name);
        free(a);
        a = next;
    }
    return 0;
}