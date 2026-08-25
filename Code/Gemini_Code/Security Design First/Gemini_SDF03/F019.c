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
    if (argc < 2) return 1;
    Transaction *arr = NULL;
    size_t count = 0;
    size_t capacity = 0;
    FILE *f = fopen(argv[1], "r");
    if (f) {
        char *acc, *mem;
        double amt;
        while (fscanf(f, "%ms %lf %ms", &acc, &amt, &mem) == 3) {
            if (count >= capacity) {
                capacity = capacity == 0 ? 4 : capacity * 2;
                Transaction *tmp = realloc(arr, capacity * sizeof(Transaction));
                if (!tmp) exit(1);
                arr = tmp;
            }
            arr[count].account = acc;
            arr[count].amount = amt;
            arr[count].memo = mem;
            count++;
        }
        fclose(f);
    }
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *acc = p;
            while (*p && *p != ' ') p++;
            char *amt_str = "0";
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                amt_str = p;
            }
            while (*p && *p != ' ') p++;
            char *memo = "";
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                memo = p;
            }
            double amt = atof(amt_str);
            if (count >= capacity) {
                capacity = capacity == 0 ? 4 : capacity * 2;
                Transaction *tmp = realloc(arr, capacity * sizeof(Transaction));
                if (!tmp) exit(1);
                arr = tmp;
            }
            arr[count].account = strdup(acc);
            arr[count].amount = amt;
            arr[count].memo = strdup(memo);
            if (!arr[count].account || !arr[count].memo) exit(1);
            count++;
        } else if (strncmp(line, "REVERSE ", 8) == 0) {
            char *p = line + 8;
            while (*p == ' ') p++;
            char *acc = p;
            while (*p && *p != ' ') p++;
            char *amt_str = "0";
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                amt_str = p;
            }
            double amt = atof(amt_str);
            if (count >= capacity) {
                capacity = capacity == 0 ? 4 : capacity * 2;
                Transaction *tmp = realloc(arr, capacity * sizeof(Transaction));
                if (!tmp) exit(1);
                arr = tmp;
            }
            arr[count].account = strdup(acc);
            arr[count].amount = -amt;
            arr[count].memo = strdup("REVERSAL");
            if (!arr[count].account || !arr[count].memo) exit(1);
            count++;
        } else if (strncmp(line, "BALANCE ", 8) == 0) {
            char *acc = line + 8;
            while (*acc == ' ') acc++;
            double total = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].account, acc) == 0) {
                    total += arr[i].amount;
                }
            }
            printf("%.2f\n", total);
        } else if (strcmp(line, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %.2f %s\n", arr[i].account, arr[i].amount, arr[i].memo);
            }
        }
    }
    for (size_t i = 0; i < count; i++) {
        free(arr[i].account);
        free(arr[i].memo);
    }
    free(arr);
    free(line);
    return 0;
}
