// F019.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *account;
    double balance;
} AccountRec;

char *read_line(FILE *fp) {
    size_t size = 16;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *nb = realloc(buf, size);
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

int main(int argc, char **argv) {
    AccountRec *list = NULL;
    size_t count = 0;
    size_t capacity = 0;
    if (argc > 1) {
        FILE *fp = fopen(argv[1], "r");
        if (fp) {
            while (1) {
                char *line = read_line(fp);
                if (!line) break;
                char *p = line;
                while (*p == ' ') p++;
                if (*p != '\0') {
                    char *account = p;
                    while (*p && *p != ' ') p++;
                    if (*p) {
                        *p = '\0';
                        p++;
                    }
                    while (*p == ' ') p++;
                    char *amount_str = p;
                    while (*p && *p != ' ') p++;
                    if (*p) {
                        *p = '\0';
                        p++;
                    }
                    double amt = atof(amount_str);
                    int found = 0;
                    for (size_t i = 0; i < count; i++) {
                        if (strcmp(list[i].account, account) == 0) {
                            list[i].balance += amt;
                            found = 1;
                            break;
                        }
                    }
                    if (!found) {
                        if (count >= capacity) {
                            capacity = capacity == 0 ? 4 : capacity * 2;
                            AccountRec *nl = realloc(list, capacity * sizeof(AccountRec));
                            if (!nl) {
                                free(line);
                                break;
                            }
                            list = nl;
                        }
                        list[count].account = strdup(account);
                        list[count].balance = amt;
                        count++;
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
        char *cmd = line;
        while (*cmd == ' ') cmd++;
        if (*cmd == '\0') {
            free(line);
            continue;
        }
        char *p = cmd;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "ADD") == 0) {
            while (*p == ' ') p++;
            char *account = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *amount_str = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            double amt = atof(amount_str);
            int found = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].account, account) == 0) {
                    list[i].balance += amt;
                    found = 1;
                    break;
                }
            }
            if (!found) {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    AccountRec *nl = realloc(list, capacity * sizeof(AccountRec));
                    if (!nl) break;
                    list = nl;
                }
                list[count].account = strdup(account);
                list[count].balance = amt;
                count++;
            }
        } else if (strcmp(cmd, "REVERSE") == 0) {
            while (*p == ' ') p++;
            char *account = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *amount_str = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            double amt = atof(amount_str);
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].account, account) == 0) {
                    list[i].balance -= amt;
                    break;
                }
            }
        } else if (strcmp(cmd, "BALANCE") == 0) {
            while (*p == ' ') p++;
            char *account = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            double bal = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].account, account) == 0) {
                    bal = list[i].balance;
                    break;
                }
            }
            printf("%.2f\n", bal);
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %.2f\n", list[i].account, list[i].balance);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(list[i].account);
    }
    free(list);
    return 0;
}
