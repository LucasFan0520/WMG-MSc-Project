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
    buf[len] = '\0';
    if (len > 0 && buf[len-1] == '\r') {
        buf[len-1] = '\0';
    }
    return buf;
}

typedef struct {
    char *account;
    double amount;
    char *memo;
} Tx;

int main(int argc, char **argv) {
    if (argc < 2) return 1;
    FILE *fp = fopen(argv[1], "r");
    if (!fp) return 1;
    Tx *txs = NULL;
    int count = 0;
    int cap = 0;
    char acc[256], mem[256], amt_str[256];
    while (fscanf(fp, "%255s %255s %255s", acc, amt_str, mem) == 3) {
        char *endptr;
        double amt = strtod(amt_str, &endptr);
        if (endptr != amt_str && *endptr == '\0') {
            if (count >= cap) {
                cap = cap == 0 ? 16 : cap * 2;
                txs = realloc(txs, cap * sizeof(Tx));
            }
            txs[count].account = strdup(acc);
            txs[count].amount = amt;
            txs[count].memo = strdup(mem);
            count++;
        }
    }
    fclose(fp);
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *account = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                char *amt_s = p;
                while (*p && *p != ' ') p++;
                if (*p == ' ') {
                    *p = '\0';
                    char *memo = p + 1;
                    while (*memo == ' ') memo++;
                    char *endptr;
                    double amt = strtod(amt_s, &endptr);
                    if (endptr != amt_s) {
                        if (count >= cap) {
                            cap = cap == 0 ? 16 : cap * 2;
                            txs = realloc(txs, cap * sizeof(Tx));
                        }
                        txs[count].account = strdup(account);
                        txs[count].amount = amt;
                        txs[count].memo = strdup(memo);
                        count++;
                    }
                }
            }
        } else if (strncmp(line, "REVERSE ", 8) == 0) {
            char *p = line + 8;
            while (*p == ' ') p++;
            char *account = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *amt_s = p + 1;
                while (*amt_s == ' ') amt_s++;
                char *endptr;
                double amt = strtod(amt_s, &endptr);
                if (endptr != amt_s) {
                    if (count >= cap) {
                        cap = cap == 0 ? 16 : cap * 2;
                        txs = realloc(txs, cap * sizeof(Tx));
                    }
                    txs[count].account = strdup(account);
                    txs[count].amount = -amt;
                    txs[count].memo = strdup("REVERSAL");
                    count++;
                }
            }
        } else if (strncmp(line, "BALANCE ", 8) == 0) {
            char *account = line + 8;
            while (*account == ' ') account++;
            double bal = 0;
            for (int i = 0; i < count; i++) {
                if (strcmp(txs[i].account, account) == 0) {
                    bal += txs[i].amount;
                }
            }
            printf("%.2f\n", bal);
        } else if (strcmp(line, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %.2f %s\n", txs[i].account, txs[i].amount, txs[i].memo);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(txs[i].account);
        free(txs[i].memo);
    }
    free(txs);
    return 0;
}
