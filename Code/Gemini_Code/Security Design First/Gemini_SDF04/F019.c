// F019.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct {
    char *account;
    int amount;
    char *memo;
} Tx;

Tx *journal = NULL;
int count = 0;
int capacity = 0;

int is_valid_number(const char *s) {
    if (!s || *s == '\0') return 0;
    if (*s == '-' || *s == '+') s++;
    if (*s == '\0') return 0;
    while (*s) {
        if (!isdigit((unsigned char)*s)) return 0;
        s++;
    }
    return 1;
}

void add_tx(const char *acc, const char *amt_str, const char *memo) {
    if (!is_valid_number(amt_str)) return;
    int amt = atoi(amt_str);
    if (count >= capacity) {
        capacity = capacity == 0 ? 4 : capacity * 2;
        Tx *tmp = realloc(journal, capacity * sizeof(Tx));
        if (!tmp) return;
        journal = tmp;
    }
    journal[count].account = strdup(acc);
    journal[count].amount = amt;
    journal[count].memo = strdup(memo);
    if (journal[count].account && journal[count].memo) {
        count++;
    }
}

int main(int argc, char **argv) {
    if (argc < 2) {
        return 1;
    }
    FILE *f = fopen(argv[1], "r");
    if (f) {
        char *fline = NULL;
        size_t flen = 0;
        ssize_t nread;
        while ((nread = getline(&fline, &flen, f)) != -1) {
            size_t l = strlen(fline);
            if (l > 0 && fline[l - 1] == '\n') fline[l - 1] = '\0';
            char *p = fline;
            while (*p && isspace((unsigned char)*p)) p++;
            if (*p == '\0') continue;
            char *acc = p;
            while (*p && !isspace((unsigned char)*p)) p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p && isspace((unsigned char)*p)) p++;
                char *amt_str = p;
                while (*p && !isspace((unsigned char)*p)) p++;
                if (*p) {
                    *p = '\0';
                    p++;
                    while (*p && isspace((unsigned char)*p)) p++;
                    char *memo = p;
                    while (*p && !isspace((unsigned char)*p)) p++;
                    if (*p) *p = '\0';
                    if (strlen(acc) > 0 && strlen(amt_str) > 0 && strlen(memo) > 0) {
                        add_tx(acc, amt_str, memo);
                    }
                }
            }
        }
        free(fline);
        fclose(f);
    }
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    while ((nread = getline(&line, &len, stdin)) != -1) {
        size_t l = strlen(line);
        if (l > 0 && line[l - 1] == '\n') line[l - 1] = '\0';
        char *cmd = line;
        char *space1 = strchr(cmd, ' ');
        if (space1) {
            *space1 = '\0';
            char *arg1 = space1 + 1;
            if (strcmp(cmd, "ADD") == 0) {
                char *space2 = strchr(arg1, ' ');
                if (space2) {
                    *space2 = '\0';
                    char *amt_str = space2 + 1;
                    char *space3 = strchr(amt_str, ' ');
                    if (space3) {
                        *space3 = '\0';
                        char *memo = space3 + 1;
                        add_tx(arg1, amt_str, memo);
                    }
                }
            } else if (strcmp(cmd, "REVERSE") == 0) {
                char *space2 = strchr(arg1, ' ');
                if (space2) {
                    *space2 = '\0';
                    char *amt_str = space2 + 1;
                    if (is_valid_number(amt_str)) {
                        int amt = atoi(amt_str);
                        if (count >= capacity) {
                            capacity = capacity == 0 ? 4 : capacity * 2;
                            Tx *tmp = realloc(journal, capacity * sizeof(Tx));
                            if (!tmp) {
                                free(line);
                                return 1;
                            }
                            journal = tmp;
                        }
                        journal[count].account = strdup(arg1);
                        journal[count].amount = -amt;
                        journal[count].memo = strdup("REVERSAL");
                        if (journal[count].account && journal[count].memo) {
                            count++;
                        }
                    }
                }
            } else if (strcmp(cmd, "BALANCE") == 0) {
                int bal = 0;
                for (int i = 0; i < count; i++) {
                    if (strcmp(journal[i].account, arg1) == 0) {
                        bal += journal[i].amount;
                    }
                }
                printf("%d\n", bal);
            }
        } else {
            if (strcmp(cmd, "REPORT") == 0) {
                for (int i = 0; i < count; i++) {
                    printf("%s %d %s\n", journal[i].account, journal[i].amount, journal[i].memo);
                }
            } else if (strcmp(cmd, "END") == 0) {
                break;
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