// F019.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t size = 1024;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_buf = realloc(buf, size);
            if (!new_buf) { free(buf); return NULL; }
            buf = new_buf;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) { free(buf); return NULL; }
    buf[len] = '\0';
    return buf;
}

typedef struct {
    char *account;
    double balance;
} Account;

int main(int argc, char **argv) {
    if (argc < 2) return 1;
    FILE *fp = fopen(argv[1], "r");
    Account *accounts = NULL;
    int count = 0;
    int capacity = 0;
    if (fp) {
        while (1) {
            char *line = read_line(fp);
            if (!line) break;
            char acc[256], mem[256];
            double amt = 0.0;
            if (sscanf(line, "%255s %lf %255s", acc, &amt, mem) == 3) {
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(accounts[i].account, acc) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    accounts[found].balance += amt;
                } else {
                    if (count >= capacity) {
                        capacity = capacity == 0 ? 4 : capacity * 2;
                        accounts = realloc(accounts, capacity * sizeof(Account));
                    }
                    accounts[count].account = strdup(acc);
                    accounts[count].balance = amt;
                    count++;
                }
            }
            free(line);
        }
        fclose(fp);
    }
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char cmd[64];
        int n = 0;
        if (sscanf(line, "%63s%n", cmd, &n) < 1) {
            free(line);
            continue;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %.2f\n", accounts[i].account, accounts[i].balance);
            }
            free(line);
            continue;
        }
        char *p = line + n;
        while (*p == ' ' || *p == '\t') p++;
        if (strcmp(cmd, "ADD") == 0) {
            char acc[256], mem[256];
            double amt = 0.0;
            if (sscanf(p, "%255s %lf %255s", acc, &amt, mem) >= 2) {
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(accounts[i].account, acc) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    accounts[found].balance += amt;
                } else {
                    if (count >= capacity) {
                        capacity = capacity == 0 ? 4 : capacity * 2;
                        accounts = realloc(accounts, capacity * sizeof(Account));
                    }
                    accounts[count].account = strdup(acc);
                    accounts[count].balance = amt;
                    count++;
                }
            }
        } else if (strcmp(cmd, "REVERSE") == 0) {
            char acc[256];
            double amt = 0.0;
            if (sscanf(p, "%255s %lf", acc, &amt) == 2) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(accounts[i].account, acc) == 0) {
                        accounts[i].balance -= amt;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "BALANCE") == 0) {
            char acc[256];
            if (sscanf(p, "%255s", acc) == 1) {
                double bal = 0.0;
                for (int i = 0; i < count; i++) {
                    if (strcmp(accounts[i].account, acc) == 0) {
                        bal = accounts[i].balance;
                        break;
                    }
                }
                printf("%.2f\n", bal);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(accounts[i].account);
    }
    free(accounts);
    return 0;
}
