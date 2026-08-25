// F019.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *account;
    double amount;
    char *memo;
} Transaction;

int main(int argc, char **argv) {
    if (argc < 2) return 1;

    Transaction *items = NULL;
    size_t count = 0;
    size_t capacity = 0;

    FILE *f = fopen(argv[1], "r");
    if (f) {
        char *fline = NULL;
        size_t flen = 0;
        long fread_bytes;
        while ((fread_bytes = getline(&fline, &flen, f)) != -1) {
            if (fread_bytes > 0 && fline[fread_bytes - 1] == '\n') {
                fline[fread_bytes - 1] = '\0';
                fread_bytes--;
            }
            if (fread_bytes > 0 && fline[fread_bytes - 1] == '\n') {
                fline[fread_bytes - 1] = '\0';
                fread_bytes--;
            }

            char *p = fline;
            while (*p == ' ' || *p == '	') p++;
            if (*p == '\0') continue;

            char *account = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '	') p++;

            char *amt_str = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '	') p++;
            char *memo = p;

            char *endptr;
            double amt = strtod(amt_str, &endptr);
            if (endptr != amt_str) {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    Transaction *new_items = realloc(items, capacity * sizeof(Transaction));
                    if (!new_items) exit(1);
                    items = new_items;
                }
                items[count].account = strdup(account);
                items[count].amount = amt;
                items[count].memo = strdup(memo);
                if (!items[count].account || !items[count].memo) exit(1);
                count++;
            }
        }
        free(fline);
        fclose(f);
    }

    char *line = NULL;
    size_t len = 0;
    long read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
            read--;
        }
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
            read--;
        }

        if (strcmp(line, "END") == 0) {
            break;
        }

        char *p = line;
        while (*p == ' ' || *p == '	') p++;
        if (*p == '\0') continue;

        char *cmd = p;
        while (*p && *p != ' ' && *p != '	') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        while (*p == ' ' || *p == '	') p++;

        if (strcmp(cmd, "ADD") == 0) {
            char *account = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '	') p++;

            char *amt_str = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '	') p++;
            char *memo = p;

            char *endptr;
            double amt = strtod(amt_str, &endptr);
            if (endptr != amt_str) {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    Transaction *new_items = realloc(items, capacity * sizeof(Transaction));
                    if (!new_items) exit(1);
                    items = new_items;
                }
                items[count].account = strdup(account);
                items[count].amount = amt;
                items[count].memo = strdup(memo);
                if (!items[count].account || !items[count].memo) exit(1);
                count++;
            }
        } else if (strcmp(cmd, "REVERSE") == 0) {
            char *account = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '	') p++;
            char *amt_str = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) *p = '\0';

            char *endptr;
            double amt = strtod(amt_str, &endptr);
            if (endptr != amt_str) {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    Transaction *new_items = realloc(items, capacity * sizeof(Transaction));
                    if (!new_items) exit(1);
                    items = new_items;
                }
                items[count].account = strdup(account);
                items[count].amount = -amt;
                items[count].memo = strdup("REVERSAL");
                if (!items[count].account || !items[count].memo) exit(1);
                count++;
            }
        } else if (strcmp(cmd, "BALANCE") == 0) {
            char *account = p;
            double bal = 0.0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].account, account) == 0) {
                    bal += items[i].amount;
                }
            }
            printf("%.2f\n", bal);
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %.2f %s\n", items[i].account, items[i].amount, items[i].memo);
            }
        }
    }

    for (size_t i = 0; i < count; i++) {
        free(items[i].account);
        free(items[i].memo);
    }
    free(items);
    free(line);
    return 0;
}
