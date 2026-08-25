// F019.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t size = 32;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
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
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

char *mystrdup(const char *s) {
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (!d) return NULL;
    strcpy(d, s);
    return d;
}

typedef struct {
    char *account;
    double balance;
} AccountBalance;

int is_valid_double(const char *s, double *val) {
    char *endptr;
    *val = strtod(s, &endptr);
    if (endptr == s) return 0;
    while (*endptr == ' ' || *endptr == '\r' || *endptr == '\n') endptr++;
    return *endptr == '\0';
}

int main(int argc, char **argv) {
    AccountBalance *list = NULL;
    size_t count = 0;
    size_t capacity = 0;
    if (argc > 1) {
        FILE *fp = fopen(argv[1], "r");
        if (fp) {
            while (1) {
                char *fline = read_line(fp);
                if (!fline) break;
                char *acc = fline;
                while (*acc == ' ') acc++;
                if (*acc != '\0') {
                    char *acc_end = acc;
                    while (*acc_end && *acc_end != ' ') acc_end++;
                    char *amt_s = acc_end;
                    if (*acc_end != '\0') {
                        *acc_end = '\0';
                        amt_s++;
                    }
                    while (*amt_s == ' ') amt_s++;
                    char *amt_end = amt_s;
                    while (*amt_end && *amt_end != ' ') amt_end++;
                    if (*amt_end != '\0') {
                        *amt_end = '\0';
                    }
                    double amt_val;
                    if (is_valid_double(amt_s, &amt_val)) {
                        int found = 0;
                        for (size_t i = 0; i < count; i++) {
                            if (strcmp(list[i].account, acc) == 0) {
                                list[i].balance += amt_val;
                                found = 1;
                                break;
                            }
                        }
                        if (!found) {
                            if (count >= capacity) {
                                capacity = capacity == 0 ? 4 : capacity * 2;
                                AccountBalance *new_list = realloc(list, capacity * sizeof(AccountBalance));
                                if (new_list) list = new_list;
                            }
                            list[count].account = mystrdup(acc);
                            list[count].balance = amt_val;
                            count++;
                        }
                    }
                }
                free(fline);
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
        while (*cmd == ' ') cmd++;
        if (*cmd == '\0') {
            free(line);
            continue;
        }
        char *p = cmd;
        while (*p && *p != ' ') p++;
        if (*p != '\0') {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "ADD") == 0) {
            char *acc = p;
            while (*acc == ' ') acc++;
            char *acc_end = acc;
            while (*acc_end && *acc_end != ' ') acc_end++;
            char *amt_s = acc_end;
            if (*acc_end != '\0') {
                *acc_end = '\0';
                amt_s++;
            }
            while (*amt_s == ' ') amt_s++;
            char *amt_end = amt_s;
            while (*amt_end && *amt_end != ' ') amt_end++;
            if (*amt_end != '\0') {
                *amt_end = '\0';
            }
            double amt_val;
            if (is_valid_double(amt_s, &amt_val)) {
                int found = 0;
                for (size_t i = 0; i < count; i++) {
                    if (strcmp(list[i].account, acc) == 0) {
                        list[i].balance += amt_val;
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    if (count >= capacity) {
                        capacity = capacity == 0 ? 4 : capacity * 2;
                        AccountBalance *new_list = realloc(list, capacity * sizeof(AccountBalance));
                        if (new_list) list = new_list;
                    }
                    list[count].account = mystrdup(acc);
                    list[count].balance = amt_val;
                    count++;
                }
            }
        } else if (strcmp(cmd, "REVERSE") == 0) {
            char *acc = p;
            while (*acc == ' ') acc++;
            char *acc_end = acc;
            while (*acc_end && *acc_end != ' ') acc_end++;
            char *amt_s = acc_end;
            if (*acc_end != '\0') {
                *acc_end = '\0';
                amt_s++;
            }
            while (*amt_s == ' ') amt_s++;
            char *amt_end = amt_s;
            while (*amt_end && *amt_end != ' ') amt_end++;
            if (*amt_end != '\0') {
                *amt_end = '\0';
            }
            double amt_val;
            if (is_valid_double(amt_s, &amt_val)) {
                int found = 0;
                for (size_t i = 0; i < count; i++) {
                    if (strcmp(list[i].account, acc) == 0) {
                        list[i].balance -= amt_val;
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    if (count >= capacity) {
                        capacity = capacity == 0 ? 4 : capacity * 2;
                        AccountBalance *new_list = realloc(list, capacity * sizeof(AccountBalance));
                        if (new_list) list = new_list;
                    }
                    list[count].account = mystrdup(acc);
                    list[count].balance = -amt_val;
                    count++;
                }
            }
        } else if (strcmp(cmd, "BALANCE") == 0) {
            char *acc = p;
            while (*acc == ' ') acc++;
            char *acc_end = acc;
            while (*acc_end && *acc_end != ' ') acc_end++;
            *acc_end = '\0';
            double bal = 0.0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].account, acc) == 0) {
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
