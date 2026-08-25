/* F019.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *account;
    double amount;
    char *memo;
} Transaction;

int main(int argc, char *argv[]) {
    if (argc < 2) return 1;
    Transaction *journal = NULL;
    int count = 0;
    int capacity = 0;

    FILE *fp = fopen(argv[1], "r");
    if (fp) {
        char *fline = NULL;
        size_t flen = 0;
        ssize_t fread_bytes;
        while ((fread_bytes = getline(&fline, &flen, fp)) != -1) {
            fline[strcspn(fline, "\r\n")] = '\0';
            char *p1 = strchr(fline, ' ');
            if (!p1) continue;
            *p1 = '\0';
            char *acc = fline;
            char *rest = p1 + 1;
            char *p2 = strchr(rest, ' ');
            if (!p2) {
                *p1 = ' ';
                continue;
            }
            *p2 = '\0';
            char *amt_str = rest;
            char *mem = p2 + 1;

            char *endptr;
            double amt = strtod(amt_str, &endptr);
            if (endptr == amt_str) {
                *p1 = ' '; *p2 = ' ';
                continue;
            }

            char *n_acc = strdup(acc);
            char *n_mem = strdup(mem);
            *p1 = ' '; *p2 = ' ';

            if (n_acc && n_mem) {
                if (count >= capacity) {
                    int new_cap = capacity == 0 ? 4 : capacity * 2;
                    Transaction *tmp = realloc(journal, new_cap * sizeof(Transaction));
                    if (tmp) {
                        journal = tmp;
                        capacity = new_cap;
                    } else {
                        free(n_acc); free(n_mem);
                        continue;
                    }
                }
                journal[count].account = n_acc;
                journal[count].amount = amt;
                journal[count].memo = n_mem;
                count++;
            } else {
                free(n_acc); free(n_mem);
            }
        }
        free(fline);
        fclose(fp);
    }

    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *args = line + 4;
            char *p1 = strchr(args, ' ');
            if (!p1) continue;
            *p1 = '\0';
            char *acc = args;
            char *rest = p1 + 1;
            char *p2 = strchr(rest, ' ');
            if (!p2) {
                *p1 = ' ';
                continue;
            }
            *p2 = '\0';
            char *amt_str = rest;
            char *mem = p2 + 1;

            char *endptr;
            double amt = strtod(amt_str, &endptr);
            if (endptr == amt_str) {
                *p1 = ' '; *p2 = ' ';
                continue;
            }

            char *n_acc = strdup(acc);
            char *n_mem = strdup(mem);
            *p1 = ' '; *p2 = ' ';

            if (n_acc && n_mem) {
                if (count >= capacity) {
                    int new_cap = capacity == 0 ? 4 : capacity * 2;
                    Transaction *tmp = realloc(journal, new_cap * sizeof(Transaction));
                    if (tmp) {
                        journal = tmp;
                        capacity = new_cap;
                    } else {
                        free(n_acc); free(n_mem);
                        continue;
                    }
                }
                journal[count].account = n_acc;
                journal[count].amount = amt;
                journal[count].memo = n_mem;
                count++;
            } else {
                free(n_acc); free(n_mem);
            }
        } else if (strncmp(line, "REVERSE ", 8) == 0) {
            char *args = line + 8;
            char *p1 = strchr(args, ' ');
            if (!p1) continue;
            *p1 = '\0';
            char *acc = args;
            char *amt_str = p1 + 1;

            char *endptr;
            double amt = strtod(amt_str, &endptr);
            if (endptr == amt_str) {
                *p1 = ' ';
                continue;
            }

            char *n_acc = strdup(acc);
            char *n_mem = strdup("REVERSAL");
            *p1 = ' ';

            if (n_acc && n_mem) {
                if (count >= capacity) {
                    int new_cap = capacity == 0 ? 4 : capacity * 2;
                    Transaction *tmp = realloc(journal, new_cap * sizeof(Transaction));
                    if (tmp) {
                        journal = tmp;
                        capacity = new_cap;
                    } else {
                        free(n_acc); free(n_mem);
                        continue;
                    }
                }
                journal[count].account = n_acc;
                journal[count].amount = -amt;
                journal[count].memo = n_mem;
                count++;
            } else {
                free(n_acc); free(n_mem);
            }
        } else if (strncmp(line, "BALANCE ", 8) == 0) {
            char *acc = line + 8;
            double bal = 0.0;
            for (int i = 0; i < count; i++) {
                if (strcmp(journal[i].account, acc) == 0) {
                    bal += journal[i].amount;
                }
            }
            printf("%.2f\n", bal);
        } else if (strcmp(line, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %.2f %s\n", journal[i].account, journal[i].amount, journal[i].memo);
            }
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(journal[i].account);
        free(journal[i].memo);
    }
    free(journal);
    return 0;
}
