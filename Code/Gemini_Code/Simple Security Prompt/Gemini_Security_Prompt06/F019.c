// F019.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *account;
    double balance;
} AccountBal;

typedef struct {
    char *account;
    double amount;
    char *memo;
} Transaction;

char *read_line(FILE *f) {
    size_t size = 32;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_buf = realloc(buf, size);
            if (!new_buf) {
                free(buf);
                return NULL;
            }
            buf = new_buf;
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

int main(int argc, char **argv) {
    AccountBal *accs = NULL;
    int a_count = 0;
    Transaction *txs = NULL;
    int t_count = 0;
    if (argc > 1) {
        FILE *fp = fopen(argv[1], "r");
        if (fp) {
            while (1) {
                char acc[256];
                char mem[256];
                double amt;
                if (fscanf(fp, "%255s %lf %255s", acc, &amt, mem) != 3) {
                    int c = fgetc(fp);
                    if (c == EOF) break;
                    continue;
                }
                int found = -1;
                for (int i = 0; i < a_count; i++) {
                    if (strcmp(accs[i].account, acc) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    accs[found].balance += amt;
                } else {
                    accs = realloc(accs, (a_count + 1) * sizeof(AccountBal));
                    accs[a_count].account = strdup(acc);
                    accs[a_count].balance = amt;
                    a_count++;
                }
                txs = realloc(txs, (t_count + 1) * sizeof(Transaction));
                txs[t_count].account = strdup(acc);
                txs[t_count].amount = amt;
                txs[t_count].memo = strdup(mem);
                t_count++;
            }
            fclose(fp);
        }
    }
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        char *p1 = strchr(line, ' ');
        if (!p1) {
            if (strcmp(line, "REPORT") == 0) {
                for (int i = 0; i < t_count; i++) {
                    printf("%s %.2f %s\n", txs[i].account, txs[i].amount, txs[i].memo);
                }
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *cmd = line;
        char *rest = p1 + 1;
        if (strcmp(cmd, "ADD") == 0) {
            char *p2 = strchr(rest, ' ');
            if (p2) {
                *p2 = '\0';
                char *account = rest;
                char *rest2 = p2 + 1;
                char *p3 = strchr(rest2, ' ');
                if (p3) {
                    *p3 = '\0';
                    char *amt_str = rest2;
                    char *memo = p3 + 1;
                    char *endptr;
                    double amt = strtod(amt_str, &endptr);
                    if (*endptr == '\0' || *endptr == ' ') {
                        int found = -1;
                        for (int i = 0; i < a_count; i++) {
                            if (strcmp(accs[i].account, account) == 0) {
                                found = i;
                                break;
                            }
                        }
                        if (found != -1) {
                            accs[found].balance += amt;
                        } else {
                            accs = realloc(accs, (a_count + 1) * sizeof(AccountBal));
                            accs[a_count].account = strdup(account);
                            accs[a_count].balance = amt;
                            a_count++;
                        }
                        txs = realloc(txs, (t_count + 1) * sizeof(Transaction));
                        txs[t_count].account = strdup(account);
                        txs[t_count].amount = amt;
                        txs[t_count].memo = strdup(memo);
                        t_count++;
                    }
                }
            }
        } else if (strcmp(cmd, "REVERSE") == 0) {
            char *p2 = strchr(rest, ' ');
            if (p2) {
                *p2 = '\0';
                char *account = rest;
                char *amt_str = p2 + 1;
                char *endptr;
                double amt = strtod(amt_str, &endptr);
                if (*endptr == '\0' || *endptr == ' ') {
                    int found = -1;
                    for (int i = 0; i < a_count; i++) {
                        if (strcmp(accs[i].account, account) == 0) {
                            found = i;
                            break;
                        }
                    }
                    if (found != -1) {
                        accs[found].balance -= amt;
                    } else {
                        accs = realloc(accs, (a_count + 1) * sizeof(AccountBal));
                        accs[a_count].account = strdup(account);
                        accs[a_count].balance = -amt;
                        a_count++;
                    }
                    txs = realloc(txs, (t_count + 1) * sizeof(Transaction));
                    txs[t_count].account = strdup(account);
                    txs[t_count].amount = -amt;
                    txs[t_count].memo = strdup("REVERSAL");
                    t_count++;
                }
            }
        } else if (strcmp(cmd, "BALANCE") == 0) {
            char *account = rest;
            int found = -1;
            for (int i = 0; i < a_count; i++) {
                if (strcmp(accs[i].account, account) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                printf("%.2f\n", accs[found].balance);
            } else {
                printf("0.00\n");
            }
        }
        free(line);
    }
    for (int i = 0; i < a_count; i++) {
        free(accs[i].account);
    }
    free(accs);
    for (int i = 0; i < t_count; i++) {
        free(txs[i].account);
        free(txs[i].memo);
    }
    free(txs);
    return 0;
}