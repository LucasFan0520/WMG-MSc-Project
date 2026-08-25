// F019.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            char *nb = realloc(buf, cap);
            if (!nb) {
                free(buf);
                return NULL;
            }
            buf = nb;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    if (len > 0 && buf[len - 1] == '\n') {
        buf[len - 1] = '\0';
    } else {
        buf[len] = '\0';
    }
    return buf;
}

char *safe_dup(const char *s) {
    size_t l = strlen(s);
    char *d = malloc(l + 1);
    if (d) {
        strcpy(d, s);
    }
    return d;
}

typedef struct {
    char *account;
    double amount;
    char *memo;
} Transaction;

int main(int argc, char **argv) {
    if (argc < 2) return 1;
    Transaction *journal = NULL;
    size_t count = 0;
    size_t cap = 0;
    FILE *fp = fopen(argv[1], "r");
    if (fp) {
        while (1) {
            char *fline = read_line(fp);
            if (!fline) break;
            char *p = fline;
            char *acc = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            char *amt_str = p;
            while (*p && *p != ' ') p++;
            char *memo = "";
            if (*p == ' ') {
                *p = '\0';
                memo = p + 1;
            }
            char *endptr;
            double amt = strtod(amt_str, &endptr);
            if (endptr != amt_str) {
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    Transaction *nb = realloc(journal, cap * sizeof(Transaction));
                    if (!nb) {
                        free(fline);
                        break;
                    }
                    journal = nb;
                }
                journal[count].account = safe_dup(acc);
                journal[count].amount = amt;
                journal[count].memo = safe_dup(memo);
                count++;
            }
            free(fline);
        }
        fclose(fp);
    }
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *p = line + 4;
            char *acc = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            char *amt_str = p;
            while (*p && *p != ' ') p++;
            char *memo = "";
            if (*p == ' ') {
                *p = '\0';
                memo = p + 1;
            }
            char *endptr;
            double amt = strtod(amt_str, &endptr);
            if (endptr != amt_str) {
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    Transaction *nb = realloc(journal, cap * sizeof(Transaction));
                    if (!nb) {
                        free(line);
                        break;
                    }
                    journal = nb;
                }
                journal[count].account = safe_dup(acc);
                journal[count].amount = amt;
                journal[count].memo = safe_dup(memo);
                count++;
            }
        } else if (strncmp(line, "REVERSE ", 8) == 0) {
            char *p = line + 8;
            char *acc = p;
            while (*p && *p != ' ') p++;
            char *amt_str = "";
            if (*p == ' ') {
                *p = '\0';
                amt_str = p + 1;
            }
            char *endptr;
            double amt = strtod(amt_str, &endptr);
            if (endptr != amt_str) {
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    Transaction *nb = realloc(journal, cap * sizeof(Transaction));
                    if (!nb) {
                        free(line);
                        break;
                    }
                    journal = nb;
                }
                journal[count].account = safe_dup(acc);
                journal[count].amount = -amt;
                journal[count].memo = safe_dup("REVERSAL");
                count++;
            }
        } else if (strncmp(line, "BALANCE ", 8) == 0) {
            char *acc = line + 8;
            double bal = 0.0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(journal[i].account, acc) == 0) {
                    bal += journal[i].amount;
                }
            }
            printf("%.2f\n", bal);
        } else if (strcmp(line, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %.2f %s\n", journal[i].account, journal[i].amount, journal[i].memo);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(journal[i].account);
        free(journal[i].memo);
    }
    free(journal);
    return 0;
}
