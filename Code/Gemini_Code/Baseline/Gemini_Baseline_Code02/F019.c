// F019.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t size = 16;
    size_t len = 0;
    char *str = malloc(size);
    if (!str) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_str = realloc(str, size);
            if (!new_str) { free(str); return NULL; }
            str = new_str;
        }
        str[len++] = c;
    }
    if (len == 0 && c == EOF) { free(str); return NULL; }
    str[len] = '\0';
    return str;
}

typedef struct {
    char *account;
    double balance;
} Acc;

int main(int argc, char **argv) {
    Acc *accounts = NULL;
    int count = 0;
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char acc[256], mem[256];
            double amt;
            while (fscanf(f, "%255s %lf %255s", acc, &amt, mem) == 3) {
                int found = 0;
                for (int i = 0; i < count; i++) {
                    if (strcmp(accounts[i].account, acc) == 0) {
                        accounts[i].balance += amt;
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    accounts = realloc(accounts, sizeof(Acc) * (count + 1));
                    accounts[count].account = strdup(acc);
                    accounts[count].balance = amt;
                    count++;
                }
            }
            fclose(f);
        }
    }
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) { free(line); break; }
        char *cmd = line;
        char *p1 = strchr(cmd, ' ');
        if (!p1) {
            if (strcmp(cmd, "REPORT") == 0) {
                for (int i = 0; i < count; i++) {
                    printf("%s %.2f\n", accounts[i].account, accounts[i].balance);
                }
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *args = p1 + 1;
        if (strcmp(cmd, "ADD") == 0) {
            char *p2 = strchr(args, ' ');
            if (p2) {
                *p2 = '\0';
                char *acc = args;
                char *amt_str = p2 + 1;
                char *p3 = strchr(amt_str, ' ');
                if (p3) {
                    *p3 = '\0';
                    char *endptr;
                    double amt = strtod(amt_str, &endptr);
                    if (endptr != amt_str) {
                        int found = 0;
                        for (int i = 0; i < count; i++) {
                            if (strcmp(accounts[i].account, acc) == 0) {
                                accounts[i].balance += amt;
                                found = 1;
                                break;
                            }
                        }
                        if (!found) {
                            accounts = realloc(accounts, sizeof(Acc) * (count + 1));
                            accounts[count].account = strdup(acc);
                            accounts[count].balance = amt;
                            count++;
                        }
                    }
                }
            }
        } else if (strcmp(cmd, "REVERSE") == 0) {
            char *p2 = strchr(args, ' ');
            if (p2) {
                *p2 = '\0';
                char *acc = args;
                char *amt_str = p2 + 1;
                char *endptr;
                double amt = strtod(amt_str, &endptr);
                if (endptr != amt_str) {
                    for (int i = 0; i < count; i++) {
                        if (strcmp(accounts[i].account, acc) == 0) {
                            accounts[i].balance -= amt;
                            break;
                        }
                    }
                }
            }
        } else if (strcmp(cmd, "BALANCE") == 0) {
            char *acc = args;
            double bal = 0.0;
            for (int i = 0; i < count; i++) {
                if (strcmp(accounts[i].account, acc) == 0) {
                    bal = accounts[i].balance;
                    break;
                }
            }
            printf("%.2f\n", bal);
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(accounts[i].account);
    }
    free(accounts);
    return 0;
}
