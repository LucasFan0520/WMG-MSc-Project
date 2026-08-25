// F019.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_word(FILE *fp) {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && (c == ' ' || c == '\n' || c == '\r' || c == '\t'));
    if (c == EOF) {
        free(buf);
        return NULL;
    }
    buf[len++] = c;
    while ((c = fgetc(fp)) != EOF && c != ' ' && c != '\n' && c != '\r' && c != '\t') {
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
    buf[len] = '\0';
    return buf;
}

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
    if (c == EOF && len == 0) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

typedef struct {
    char *account;
    double balance;
} Account;

Account *items = NULL;
size_t count = 0;
size_t cap = 0;

void adjust_balance(char *account, double amount) {
    for (size_t i = 0; i < count; i++) {
        if (strcmp(items[i].account, account) == 0) {
            items[i].balance += amount;
            return;
        }
    }
    if (count >= cap) {
        cap = cap == 0 ? 16 : cap * 2;
        Account *nb = realloc(items, cap * sizeof(Account));
        if (nb) items = nb;
    }
    items[count].account = strdup(account);
    items[count].balance = amount;
    count++;
}

int main(int argc, char **argv) {
    if (argc >= 2) {
        FILE *fp = fopen(argv[1], "r");
        if (fp) {
            while (1) {
                char *acct = read_word(fp);
                if (!acct) break;
                char *amt_str = read_word(fp);
                char *memo = read_word(fp);
                if (!amt_str || !memo) {
                    free(acct);
                    free(amt_str);
                    free(memo);
                    break;
                }
                char *endptr;
                double amt = strtod(amt_str, &endptr);
                if (*endptr == '\0') {
                    adjust_balance(acct, amt);
                }
                free(acct);
                free(amt_str);
                free(memo);
            }
            fclose(fp);
        }
    }
    while (1) {
        char *line = read_line(stdin);
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
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %.2f\n", items[i].account, items[i].balance);
            }
            free(line);
            continue;
        }
        while (*p == ' ') p++;
        if (strcmp(cmd, "ADD") == 0) {
            char *acct = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *amt_str = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *memo = p;
            char *endptr;
            double amt = strtod(amt_str, &endptr);
            if (*endptr == '\0') {
                adjust_balance(acct, amt);
            }
        } else if (strcmp(cmd, "REVERSE") == 0) {
            char *acct = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *amt_str = p;
            char *endptr;
            double amt = strtod(amt_str, &endptr);
            if (*endptr == '\0') {
                adjust_balance(acct, -amt);
            }
        } else if (strcmp(cmd, "BALANCE") == 0) {
            char *acct = p;
            double bal = 0.0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].account, acct) == 0) {
                    bal = items[i].balance;
                    break;
                }
            }
            printf("%.2f\n", bal);
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(items[i].account);
    }
    free(items);
    return 0;
}
