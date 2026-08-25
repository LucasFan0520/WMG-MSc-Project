// F019.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t cap = 128;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            size_t ncap = cap * 2;
            char *nb = realloc(buf, ncap);
            if (!nb) {
                free(buf);
                return NULL;
            }
            buf = nb;
            cap = ncap;
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
} AccountBal;

void update_balance(AccountBal **arr, size_t *count, size_t *cap, const char *acc, double amt) {
    int found = -1;
    for (size_t i = 0; i < *count; i++) {
        if (strcmp((*arr)[i].account, acc) == 0) {
            found = (int)i;
            break;
        }
    }
    if (found != -1) {
        (*arr)[found].balance += amt;
    } else {
        if (*count >= *cap) {
            size_t ncap = *cap == 0 ? 4 : *cap * 2;
            AccountBal *narr = realloc(*arr, ncap * sizeof(AccountBal));
            if (narr) {
                *arr = narr;
                *cap = ncap;
            }
        }
        if (*count < *cap) {
            (*arr)[*count].account = strdup(acc);
            (*arr)[*count].balance = amt;
            (*count)++;
        }
    }
}

int main(int argc, char **argv) {
    AccountBal *arr = NULL;
    size_t count = 0;
    size_t cap = 0;
    if (argc > 1) {
        FILE *fp = fopen(argv[1], "r");
        if (fp) {
            char acc[256], amt_s[256], memo[256];
            while (fscanf(fp, "%255s %255s %255s", acc, amt_s, memo) == 3) {
                char *endptr;
                double amt = strtod(amt_s, &endptr);
                if (endptr != amt_s && (*endptr == '\0')) {
                    update_balance(&arr, &count, &cap, acc, amt);
                }
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
        char *cmd = line;
        char *p1 = strchr(cmd, ' ');
        if (!p1) {
            if (strcmp(cmd, "REPORT") == 0) {
                for (size_t i = 0; i < count; i++) {
                    printf("%s %.2f\n", arr[i].account, arr[i].balance);
                }
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *args = p1 + 1;
        if (strcmp(cmd, "ADD") == 0) {
            char *acc = args;
            char *p2 = strchr(acc, ' ');
            if (p2) {
                *p2 = '\0';
                char *amt_s = p2 + 1;
                char *p3 = strchr(amt_s, ' ');
                if (p3) {
                    *p3 = '\0';
                    char *endptr;
                    double amt = strtod(amt_s, &endptr);
                    if (endptr != amt_s && *endptr == '\0') {
                        update_balance(&arr, &count, &cap, acc, amt);
                    }
                }
            }
        } else if (strcmp(cmd, "REVERSE") == 0) {
            char *acc = args;
            char *p2 = strchr(acc, ' ');
            if (p2) {
                *p2 = '\0';
                char *amt_s = p2 + 1;
                char *endptr;
                double amt = strtod(amt_s, &endptr);
                if (endptr != amt_s && *endptr == '\0') {
                    update_balance(&arr, &count, &cap, acc, -amt);
                }
            }
        } else if (strcmp(cmd, "BALANCE") == 0) {
            char *acc = args;
            double bal = 0.0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].account, acc) == 0) {
                    bal = arr[i].balance;
                    break;
                }
            }
            printf("%.2f\n", bal);
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(arr[i].account);
    }
    free(arr);
    return 0;
}
