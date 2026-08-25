// F019.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t cap = 128;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
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

char *mystrdup(const char *s) {
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (d) strcpy(d, s);
    return d;
}

typedef struct {
    char *account;
    double balance;
} AccountBal;

typedef struct {
    char *account;
    double amount;
    char *memo;
} Transaction;

int main(int argc, char **argv) {
    AccountBal *bals = NULL;
    size_t b_count = 0;
    size_t b_cap = 0;
    Transaction *txs = NULL;
    size_t t_count = 0;
    size_t t_cap = 0;
    if (argc > 1) {
        FILE *fp = fopen(argv[1], "r");
        if (fp) {
            while (1) {
                char *line = read_line(fp);
                if (!line) break;
                char acct[128], memo[128];
                double amt;
                if (sscanf(line, "%127s %lf %127s", acct, &amt, memo) == 3) {
                    if (t_count >= t_cap) {
                        t_cap = t_cap == 0 ? 4 : t_cap * 2;
                        txs = realloc(txs, t_cap * sizeof(Transaction));
                    }
                    txs[t_count].account = mystrdup(acct);
                    txs[t_count].amount = amt;
                    txs[t_count].memo = mystrdup(memo);
                    t_count++;
                    int found = 0;
                    for (size_t i = 0; i < b_count; i++) {
                        if (strcmp(bals[i].account, acct) == 0) {
                            bals[i].balance += amt;
                            found = 1;
                            break;
                        }
                    }
                    if (!found) {
                        if (b_count >= b_cap) {
                            b_cap = b_cap == 0 ? 4 : b_cap * 2;
                            bals = realloc(bals, b_cap * sizeof(AccountBal));
                        }
                        bals[b_count].account = mystrdup(acct);
                        bals[b_count].balance = amt;
                        b_count++;
                    }
                }
                free(line);
            }
            fclose(fp);
        }
    }
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
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
        while (*p == ' ') p++;
        char *account = p;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        while (*p == ' ') p++;
        char *amt_s = p;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        while (*p == ' ') p++;
        char *memo = p;
        if (strcmp(cmd, "ADD") == 0) {
            double amt = atof(amt_s);
            if (t_count >= t_cap) {
                t_cap = t_cap == 0 ? 4 : t_cap * 2;
                txs = realloc(txs, t_cap * sizeof(Transaction));
            }
            txs[t_count].account = mystrdup(account);
            txs[t_count].amount = amt;
            txs[t_count].memo = mystrdup(memo);
            t_count++;
            int found = 0;
            for (size_t i = 0; i < b_count; i++) {
                if (strcmp(bals[i].account, account) == 0) {
                    bals[i].balance += amt;
                    found = 1;
                    break;
                }
            }
            if (!found) {
                if (b_count >= b_cap) {
                    b_cap = b_cap == 0 ? 4 : b_cap * 2;
                    bals = realloc(bals, b_cap * sizeof(AccountBal));
                }
                bals[b_count].account = mystrdup(account);
                bals[b_count].balance = amt;
                b_count++;
            }
        } else if (strcmp(cmd, "REVERSE") == 0) {
            double amt = atof(amt_s);
            for (size_t i = 0; i < b_count; i++) {
                if (strcmp(bals[i].account, account) == 0) {
                    bals[i].balance -= amt;
                    break;
                }
            }
        } else if (strcmp(cmd, "BALANCE") == 0) {
            int found = 0;
            for (size_t i = 0; i < b_count; i++) {
                if (strcmp(bals[i].account, account) == 0) {
                    printf("%.2f\n", bals[i].balance);
                    found = 1;
                    break;
                }
            }
            if (!found) {
                printf("0.00\n");
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < t_count; i++) {
                printf("%s %.2f %s\n", txs[i].account, txs[i].amount, txs[i].memo);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < b_count; i++) {
        free(bals[i].account);
    }
    free(bals);
    for (size_t i = 0; i < t_count; i++) {
        free(txs[i].account);
        free(txs[i].memo);
    }
    free(txs);
    return 0;
}
