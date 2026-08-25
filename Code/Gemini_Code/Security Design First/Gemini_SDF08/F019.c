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
    Transaction *list = NULL;
    int count = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            while ((read = getline(&line, &len, f)) != -1) {
                if (read > 0 && line[read - 1] == '\n') {
                    line[read - 1] = '\0';
                }
                char *s1 = strchr(line, ' ');
                if (s1) {
                    *s1 = '\0';
                    char *acc = line;
                    char *p2 = s1 + 1;
                    char *s2 = strchr(p2, ' ');
                    if (s2) {
                        *s2 = '\0';
                        char *amt_str = p2;
                        char *memo = s2 + 1;
                        char *endptr;
                        double amt = strtod(amt_str, &endptr);
                        if (*endptr == '\0' || *endptr == ' ' || *endptr == '\r') {
                            Transaction *tmp = realloc(list, sizeof(Transaction) * (count + 1));
                            if (tmp) {
                                list = tmp;
                                list[count].account = strdup(acc);
                                list[count].amount = amt;
                                list[count].memo = strdup(memo);
                                if (list[count].account && list[count].memo) {
                                    count++;
                                }
                            }
                        }
                    }
                }
            }
            fclose(f);
        }
    }

    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *p = line + 4;
            char *s1 = strchr(p, ' ');
            if (s1) {
                *s1 = '\0';
                char *acc = p;
                char *p2 = s1 + 1;
                char *s2 = strchr(p2, ' ');
                if (s2) {
                    *s2 = '\0';
                    char *amt_str = p2;
                    char *memo = s2 + 1;
                    char *endptr;
                    double amt = strtod(amt_str, &endptr);
                    if (*endptr == '\0') {
                        Transaction *tmp = realloc(list, sizeof(Transaction) * (count + 1));
                        if (tmp) {
                            list = tmp;
                            list[count].account = strdup(acc);
                            list[count].amount = amt;
                            list[count].memo = strdup(memo);
                            if (list[count].account && list[count].memo) {
                                count++;
                            }
                        }
                    }
                }
            }
        } else if (strncmp(line, "REVERSE ", 8) == 0) {
            char *p = line + 8;
            char *space = strchr(p, ' ');
            if (space) {
                *space = '\0';
                char *acc = p;
                char *amt_str = space + 1;
                char *endptr;
                double amt = strtod(amt_str, &endptr);
                if (*endptr == '\0') {
                    Transaction *tmp = realloc(list, sizeof(Transaction) * (count + 1));
                    if (tmp) {
                        list = tmp;
                        list[count].account = strdup(acc);
                        list[count].amount = -amt;
                        list[count].memo = strdup("REVERSAL");
                        if (list[count].account && list[count].memo) {
                            count++;
                        }
                    }
                }
            }
        } else if (strncmp(line, "BALANCE ", 8) == 0) {
            char *acc = line + 8;
            double bal = 0.0;
            for (int i = 0; i < count; i++) {
                if (strcmp(list[i].account, acc) == 0) {
                    bal += list[i].amount;
                }
            }
            printf("%.2f\n", bal);
        } else if (strcmp(line, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %.2f %s\n", list[i].account, list[i].amount, list[i].memo);
            }
        }
    }
    for (int i = 0; i < count; i++) {
        free(list[i].account);
        free(list[i].memo);
    }
    free(list);
    free(line);
    return 0;
}
