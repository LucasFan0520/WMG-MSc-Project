// F019.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *account;
    double balance;
} Account;

int main(int argc, char **argv) {
    if (argc < 2) return 1;
    Account *list = NULL;
    size_t count = 0;
    size_t capacity = 0;

    FILE *f = fopen(argv[1], "r");
    if (f) {
        char *fline = NULL;
        size_t flen = 0;
        ssize_t fread;
        while ((fread = getline(&fline, &flen, f)) != -1) {
            fline[strcspn(fline, "\r\n")] = '\0';
            char *sp1 = strchr(fline, ' ');
            if (sp1) {
                *sp1 = '\0';
                char *acc = fline;
                char *rest = sp1 + 1;
                char *sp2 = strchr(rest, ' ');
                if (sp2) {
                    *sp2 = '\0';
                    char *amt_str = rest;
                    char *endptr;
                    double val = strtod(amt_str, &endptr);
                    if (endptr != amt_str) {
                        int found = -1;
                        for (size_t i = 0; i < count; i++) {
                            if (strcmp(list[i].account, acc) == 0) {
                                found = (int)i;
                                break;
                            }
                        }
                        if (found != -1) {
                            list[found].balance += val;
                        } else {
                            if (count >= capacity) {
                                capacity = capacity == 0 ? 4 : capacity * 2;
                                Account *next = realloc(list, capacity * sizeof(Account));
                                if (!next) break;
                                list = next;
                            }
                            list[count].account = strdup(acc);
                            list[count].balance = val;
                            count++;
                        }
                    }
                }
            }
        }
        free(fline);
        fclose(f);
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
            char *sp1 = strchr(args, ' ');
            if (sp1) {
                *sp1 = '\0';
                char *acc = args;
                char *rest = sp1 + 1;
                char *sp2 = strchr(rest, ' ');
                if (sp2) {
                    *sp2 = '\0';
                    char *amt_str = rest;
                    char *endptr;
                    double val = strtod(amt_str, &endptr);
                    if (endptr != amt_str) {
                        int found = -1;
                        for (size_t i = 0; i < count; i++) {
                            if (strcmp(list[i].account, acc) == 0) {
                                found = (int)i;
                                break;
                            }
                        }
                        if (found != -1) {
                            list[found].balance += val;
                        } else {
                            if (count >= capacity) {
                                capacity = capacity == 0 ? 4 : capacity * 2;
                                Account *next = realloc(list, capacity * sizeof(Account));
                                if (!next) break;
                                list = next;
                            }
                            list[count].account = strdup(acc);
                            list[count].balance = val;
                            count++;
                        }
                    }
                }
            }
        } else if (strncmp(line, "REVERSE ", 8) == 0) {
            char *args = line + 8;
            char *space = strchr(args, ' ');
            if (space) {
                *space = '\0';
                char *acc = args;
                char *amt_str = space + 1;
                char *endptr;
                double val = strtod(amt_str, &endptr);
                if (endptr != amt_str) {
                    for (size_t i = 0; i < count; i++) {
                        if (strcmp(list[i].account, acc) == 0) {
                            list[i].balance -= val;
                            break;
                        }
                    }
                }
            }
        } else if (strncmp(line, "BALANCE ", 8) == 0) {
            char *acc = line + 8;
            double bal = 0.0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].account, acc) == 0) {
                    bal = list[i].balance;
                    break;
                }
            }
            printf("%.2f\n", bal);
        } else if (strcmp(line, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %.2f\n", list[i].account, list[i].balance);
            }
        }
    }
    free(line);
    for (size_t i = 0; i < count; i++) {
        free(list[i].account);
    }
    free(list);
    return 0;
}
