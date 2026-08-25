/* F019.c */
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
        len--;
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
    double balance;
} Account;

int main(int argc, char **argv) {
    if (argc < 2) return 1;
    Account *table = NULL;
    size_t count = 0;
    size_t cap = 0;
    FILE *fp = fopen(argv[1], "r");
    if (fp) {
        while (1) {
            char *line = read_line(fp);
            if (!line) break;
            char *p = line;
            while (*p == ' ') p++;
            if (*p != '\0') {
                char *acc = p;
                while (*p && *p != ' ') p++;
                if (*p == ' ') { *p = '\0'; p++; }
                while (*p == ' ') p++;
                char *amt_str = p;
                while (*p && *p != ' ') p++;
                if (*p == ' ') { *p = '\0'; p++; }
                while (*p == ' ') p++;
                char *memo = p;
                while (*p && *p != ' ') p++;
                if (*p == ' ') { *p = '\0'; p++; }
                char *endptr;
                double val = strtod(amt_str, &endptr);
                if (endptr != amt_str) {
                    int found = -1;
                    for (size_t i = 0; i < count; i++) {
                        if (strcmp(table[i].account, acc) == 0) {
                            found = (int)i;
                            break;
                        }
                    }
                    if (found != -1) {
                        table[found].balance += val;
                    } else {
                        if (count >= cap) {
                            cap = cap == 0 ? 4 : cap * 2;
                            Account *nt = realloc(table, cap * sizeof(Account));
                            if (nt) table = nt;
                        }
                        char *na = mystrdup(acc);
                        if (na) {
                            table[count].account = na;
                            table[count].balance = val;
                            count++;
                        }
                    }
                }
            }
            free(line);
        }
        fclose(fp);
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
        if (*p == ' ') {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "ADD") == 0) {
            while (*p == ' ') p++;
            char *acc = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *amt_str = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *memo = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') { *p = '\0'; p++; }
            char *endptr;
            double val = strtod(amt_str, &endptr);
            if (endptr != amt_str) {
                int found = -1;
                for (size_t i = 0; i < count; i++) {
                    if (strcmp(table[i].account, acc) == 0) {
                        found = (int)i;
                        break;
                    }
                }
                if (found != -1) {
                    table[found].balance += val;
                } else {
                    if (count >= cap) {
                        cap = cap == 0 ? 4 : cap * 2;
                        Account *nt = realloc(table, cap * sizeof(Account));
                        if (nt) table = nt;
                    }
                    char *na = mystrdup(acc);
                    if (na) {
                        table[count].account = na;
                        table[count].balance = val;
                        count++;
                    }
                }
            }
        } else if (strcmp(cmd, "REVERSE") == 0) {
            while (*p == ' ') p++;
            char *acc = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *amt_str = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') { *p = '\0'; p++; }
            char *endptr;
            double val = strtod(amt_str, &endptr);
            if (endptr != amt_str) {
                int found = -1;
                for (size_t i = 0; i < count; i++) {
                    if (strcmp(table[i].account, acc) == 0) {
                        found = (int)i;
                        break;
                    }
                }
                if (found != -1) {
                    table[found].balance -= val;
                }
            }
        } else if (strcmp(cmd, "BALANCE") == 0) {
            while (*p == ' ') p++;
            char *acc = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') { *p = '\0'; p++; }
            double bal = 0.0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(table[i].account, acc) == 0) {
                    bal = table[i].balance;
                    break;
                }
            }
            printf("%.2f\n", bal);
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %.2f\n", table[i].account, table[i].balance);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(table[i].account);
    }
    free(table);
    return 0;
}
