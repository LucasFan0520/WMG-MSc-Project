// F019.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *account;
    double amount;
    char *memo;
} Transaction;

int main(int argc, char **argv) {
    if (argc < 2) {
        return 1;
    }

    Transaction *journal = NULL;
    size_t count = 0;
    size_t capacity = 0;

    FILE *fp = fopen(argv[1], "r");
    if (!fp) {
        return 1;
    }

    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, fp)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0') continue;
        char *account = p;
        while (*p && *p != ' ' && *p != '\t') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        while (*p == ' ' || *p == '\t') p++;
        char *amt_str = p;
        while (*p && *p != ' ' && *p != '\t') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        while (*p == ' ' || *p == '\t') p++;
        char *memo = p;
        while (*p && *p != ' ' && *p != '\t') p++;
        if (*p) *p = '\0';

        if (*account == '\0' || *amt_str == '\0') continue;

        char *endptr;
        double amt = strtod(amt_str, &endptr);
        if (endptr == amt_str || *endptr != '\0') continue;

        if (count >= capacity) {
            size_t new_cap = capacity == 0 ? 4 : capacity * 2;
            Transaction *new_j = realloc(journal, new_cap * sizeof(Transaction));
            if (!new_j) continue;
            journal = new_j;
            capacity = new_cap;
        }

        char *acc = strdup(account);
        char *mem = strdup(memo);
        if (acc && mem) {
            journal[count].account = acc;
            journal[count].amount = amt;
            journal[count].memo = mem;
            count++;
        } else {
            free(acc);
            free(mem);
        }
    }
    fclose(fp);

    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0') continue;
        char *cmd = p;
        while (*p && *p != ' ' && *p != '\t') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            break;
        }
        if (strcmp(cmd, "ADD") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *account = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *amt_str = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *memo = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';

            if (*account == '\0' || *amt_str == '\0') continue;

            char *endptr;
            double amt = strtod(amt_str, &endptr);
            if (endptr == amt_str || *endptr != '\0') continue;

            if (count >= capacity) {
                size_t new_cap = capacity == 0 ? 4 : capacity * 2;
                Transaction *new_j = realloc(journal, new_cap * sizeof(Transaction));
                if (!new_j) continue;
                journal = new_j;
                capacity = new_cap;
            }
            char *acc = strdup(account);
            char *mem = strdup(memo);
            if (acc && mem) {
                journal[count].account = acc;
                journal[count].amount = amt;
                journal[count].memo = mem;
                count++;
            } else {
                free(acc);
                free(mem);
            }
        } else if (strcmp(cmd, "REVERSE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *account = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *amt_str = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';

            if (*account == '\0' || *amt_str == '\0') continue;

            char *endptr;
            double amt = strtod(amt_str, &endptr);
            if (endptr == amt_str || *endptr != '\0') continue;

            if (count >= capacity) {
                size_t new_cap = capacity == 0 ? 4 : capacity * 2;
                Transaction *new_j = realloc(journal, new_cap * sizeof(Transaction));
                if (!new_j) continue;
                journal = new_j;
                capacity = new_cap;
            }
            char *acc = strdup(account);
            char *mem = strdup("REVERSE");
            if (acc && mem) {
                journal[count].account = acc;
                journal[count].amount = -amt;
                journal[count].memo = mem;
                count++;
            } else {
                free(acc);
                free(mem);
            }
        } else if (strcmp(cmd, "BALANCE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *account = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            if (*account == '\0') continue;

            double bal = 0.0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(journal[i].account, account) == 0) {
                    bal += journal[i].amount;
                }
            }
            printf("%.2f\n", bal);
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %.2f %s\n", journal[i].account, journal[i].amount, journal[i].memo);
            }
        }
    }

    for (size_t i = 0; i < count; i++) {
        free(journal[i].account);
        free(journal[i].memo);
    }
    free(journal);
    free(line);
    return 0;
}
