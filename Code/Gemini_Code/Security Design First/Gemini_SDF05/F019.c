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
    Account *journal = NULL;
    int count = 0;
    int capacity = 0;

    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char *fline = NULL;
            size_t flen = 0;
            ssize_t fread;
            while ((fread = getline(&fline, &flen, f)) != -1) {
                if (fread > 0 && fline[fread - 1] == '\n') fline[fread - 1] = '\0';
                char *p = fline;
                while (*p == ' ') p++;
                if (*p == '\0') continue;
                char *account = p;
                char *space1 = strchr(account, ' ');
                if (!space1) continue;
                *space1 = '\0';
                char *amt_str = space1 + 1;
                while (*amt_str == ' ') amt_str++;
                char *space2 = strchr(amt_str, ' ');
                if (space2) *space2 = '\0';
                char *endptr;
                double amt = strtod(amt_str, &endptr);
                if (amt_str == endptr) continue;
                
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(journal[i].account, account) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    journal[found].balance += amt;
                } else {
                    if (count >= capacity) {
                        int new_cap = capacity == 0 ? 4 : capacity * 2;
                        Account *tmp = realloc(journal, new_cap * sizeof(Account));
                        if (!tmp) return 1;
                        journal = tmp;
                        capacity = new_cap;
                    }
                    journal[count].account = strdup(account);
                    journal[count].balance = amt;
                    count++;
                }
            }
            free(fline);
            fclose(f);
        }
    }

    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') line[read - 1] = '\0';
        if (strcmp(line, "END") == 0) break;
        if (strncmp(line, "ADD ", 4) == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *account = p;
            char *space1 = strchr(account, ' ');
            if (!space1) continue;
            *space1 = '\0';
            char *amt_str = space1 + 1;
            while (*amt_str == ' ') amt_str++;
            char *space2 = strchr(amt_str, ' ');
            if (space2) *space2 = '\0';
            char *endptr;
            double amt = strtod(amt_str, &endptr);
            if (amt_str == endptr) continue;
            
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(journal[i].account, account) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                journal[found].balance += amt;
            } else {
                if (count >= capacity) {
                    int new_cap = capacity == 0 ? 4 : capacity * 2;
                    Account *tmp = realloc(journal, new_cap * sizeof(Account));
                    if (!tmp) return 1;
                    journal = tmp;
                    capacity = new_cap;
                }
                journal[count].account = strdup(account);
                journal[count].balance = amt;
                count++;
            }
        } else if (strncmp(line, "REVERSE ", 8) == 0) {
            char *p = line + 8;
            while (*p == ' ') p++;
            char *account = p;
            char *space1 = strchr(account, ' ');
            if (!space1) continue;
            *space1 = '\0';
            char *amt_str = space1 + 1;
            while (*amt_str == ' ') amt_str++;
            char *endptr;
            double amt = strtod(amt_str, &endptr);
            if (amt_str == endptr) continue;
            
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(journal[i].account, account) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                journal[found].balance -= amt;
            }
        } else if (strncmp(line, "BALANCE ", 8) == 0) {
            char *account = line + 8;
            while (*account == ' ') account++;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(journal[i].account, account) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) printf("%.2f\n", journal[found].balance);
            else printf("0.00\n");
        } else if (strcmp(line, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %.2f\n", journal[i].account, journal[i].balance);
            }
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(journal[i].account);
    }
    free(journal);
    return 0;
}
