// F019.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line_from(FILE *fp) {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            size_t ncap = cap * 2;
            char *nbuf = realloc(buf, ncap);
            if (!nbuf) {
                free(buf);
                return NULL;
            }
            buf = nbuf;
            cap = ncap;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

char *mystrdup(const char *s) {
    size_t l = strlen(s);
    char *d = malloc(l + 1);
    if (d) strcpy(d, s);
    return d;
}

typedef struct {
    char *account;
    double amount;
    char *memo;
} Transaction;

int main(int argc, char **argv) {
    Transaction *items = NULL;
    size_t count = 0;
    size_t cap = 0;
    if (argc > 1) {
        FILE *fp = fopen(argv[1], "r");
        if (fp) {
            while (1) {
                char *line = read_line_from(fp);
                if (!line) break;
                char *p = line;
                while (*p == ' ') p++;
                if (*p != '\0') {
                    char *acc = p;
                    while (*p && *p != ' ') p++;
                    if (*p) {
                        *p = '\0';
                        p++;
                        while (*p == ' ') p++;
                    }
                    char *amt_s = p;
                    while (*p && *p != ' ') p++;
                    if (*p) {
                        *p = '\0';
                        p++;
                        while (*p == ' ') p++;
                    }
                    char *memo = p;
                    size_t mlen = strlen(memo);
                    while (mlen > 0 && (memo[mlen-1] == '\n' || memo[mlen-1] == '\n')) {
                        memo[mlen-1] = '\0';
                        mlen--;
                    }
                    char *endptr;
                    double amt = strtod(amt_s, &endptr);
                    if (endptr != amt_s) {
                        if (count >= cap) {
                            cap = cap == 0 ? 8 : cap * 2;
                            Transaction *nitems = realloc(items, cap * sizeof(Transaction));
                            if (nitems) items = nitems;
                        }
                        items[count].account = mystrdup(acc);
                        items[count].amount = amt;
                        items[count].memo = mystrdup(memo);
                        count++;
                    }
                }
                free(line);
            }
            fclose(fp);
        }
    }
    while (1) {
        char *line = read_line_from(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
            while (*p == ' ') p++;
        }
        size_t clen = strlen(cmd);
        while (clen > 0 && (cmd[clen-1] == '\n' || cmd[clen-1] == '\n')) {
            cmd[clen-1] = '\0';
            clen--;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "ADD") == 0) {
            char *acc = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
            }
            char *amt_s = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
            }
            char *memo = p;
            size_t mlen = strlen(memo);
            while (mlen > 0 && (memo[mlen-1] == '\n' || memo[mlen-1] == '\n')) {
                memo[mlen-1] = '\0';
                mlen--;
            }
            char *endptr;
            double amt = strtod(amt_s, &endptr);
            if (endptr != amt_s) {
                if (count >= cap) {
                    cap = cap == 0 ? 8 : cap * 2;
                    Transaction *nitems = realloc(items, cap * sizeof(Transaction));
                    if (nitems) items = nitems;
                }
                items[count].account = mystrdup(acc);
                items[count].amount = amt;
                items[count].memo = mystrdup(memo);
                count++;
            }
        } else if (strcmp(cmd, "REVERSE") == 0) {
            char *acc = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
            }
            char *amt_s = p;
            size_t amlen = strlen(amt_s);
            while (amlen > 0 && (amt_s[amlen-1] == '\n' || amt_s[amlen-1] == '\n')) {
                amt_s[amlen-1] = '\0';
                amlen--;
            }
            char *endptr;
            double amt = strtod(amt_s, &endptr);
            if (endptr != amt_s) {
                if (count >= cap) {
                    cap = cap == 0 ? 8 : cap * 2;
                    Transaction *nitems = realloc(items, cap * sizeof(Transaction));
                    if (nitems) items = nitems;
                }
                items[count].account = mystrdup(acc);
                items[count].amount = -amt;
                items[count].memo = mystrdup("REVERSE");
                count++;
            }
        } else if (strcmp(cmd, "BALANCE") == 0) {
            char *acc = p;
            size_t aclen = strlen(acc);
            while (aclen > 0 && (acc[aclen-1] == '\n' || acc[aclen-1] == '\n')) {
                acc[aclen-1] = '\0';
                aclen--;
            }
            double bal = 0.0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].account, acc) == 0) {
                    bal += items[i].amount;
                }
            }
            printf("%.2f\n", bal);
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %.2f %s\n", items[i].account, items[i].amount, items[i].memo);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(items[i].account);
        free(items[i].memo);
    }
    free(items);
    return 0;
}
