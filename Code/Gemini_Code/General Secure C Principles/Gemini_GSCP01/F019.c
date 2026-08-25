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
    Transaction *list = NULL;
    size_t count = 0;
    FILE *fp = fopen(argv[1], "r");
    char *line = NULL;
    size_t line_len = 0;
    ssize_t read;
    if (fp) {
        while ((read = getline(&line, &line_len, fp)) != -1) {
            if (read > 0 && line[read - 1] == '\n') line[read - 1] = '\0';
            if (read > 1 && line[read - 2] == '\r') line[read - 2] = '\0';
            char *p = line;
            while (*p == ' ' || *p == '\t') p++;
            if (*p == '\0') continue;
            char *acc = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ' || *p == '\t') p++;
            char *amt_s = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ' || *p == '\t') p++;
            char *memo = p;
            char *endptr;
            double amt = strtod(amt_s, &endptr);
            if (endptr == amt_s) continue;
            Transaction *temp = realloc(list, (count + 1) * sizeof(Transaction));
            if (temp) {
                list = temp;
                list[count].account = strdup(acc);
                list[count].amount = amt;
                list[count].memo = strdup(memo);
                count++;
            }
        }
        fclose(fp);
    }
    while ((read = getline(&line, &line_len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') line[read - 1] = '\0';
        if (read > 1 && line[read - 2] == '\r') line[read - 2] = '\0';
        char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0') continue;
        char *cmd = p;
        while (*p && *p != ' ' && *p != '\t') p++;
        if (*p) { *p = '\0'; p++; }
        if (strcmp(cmd, "END") == 0) {
            break;
        }
        if (strcmp(cmd, "ADD") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *acc = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ' || *p == '\t') p++;
            char *amt_s = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ' || *p == '\t') p++;
            char *memo = p;
            char *endptr;
            double amt = strtod(amt_s, &endptr);
            if (endptr == amt_s) continue;
            Transaction *temp = realloc(list, (count + 1) * sizeof(Transaction));
            if (temp) {
                list = temp;
                list[count].account = strdup(acc);
                list[count].amount = amt;
                list[count].memo = strdup(memo);
                count++;
            }
        } else if (strcmp(cmd, "REVERSE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *acc = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ' || *p == '\t') p++;
            char *amt_s = p;
            char *endptr;
            double amt = strtod(amt_s, &endptr);
            if (endptr == amt_s) continue;
            Transaction *temp = realloc(list, (count + 1) * sizeof(Transaction));
            if (temp) {
                list = temp;
                list[count].account = strdup(acc);
                list[count].amount = -amt;
                list[count].memo = strdup("REVERSAL");
                count++;
            }
        } else if (strcmp(cmd, "BALANCE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *acc = p;
            double bal = 0.0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].account, acc) == 0) {
                    bal += list[i].amount;
                }
            }
            printf("%.2f\n", bal);
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %.2f %s\n", list[i].account, list[i].amount, list[i].memo);
            }
        }
    }
    for (size_t i = 0; i < count; i++) {
        free(list[i].account);
        free(list[i].memo);
    }
    free(list);
    free(line);
    return 0;
}
