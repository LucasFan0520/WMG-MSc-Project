// F019.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *stream) {
    size_t size = 16;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(stream)) != EOF && c != '\n') {
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
    if (len > 0 && buf[len - 1] == '\r') {
        buf[len - 1] = '\0';
    }
    return buf;
}

typedef struct {
    char *account;
    double balance;
} Account;

int main(int argc, char **argv) {
    Account *entries = NULL;
    size_t count = 0;
    size_t capacity = 0;
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            while (1) {
                char *fline = read_line(f);
                if (!fline) break;
                char *p = fline;
                while (*p == ' ' || *p == '\t') p++;
                char *acc = p;
                while (*p && *p != ' ' && *p != '\t') p++;
                if (*p) {
                    *p = '\0';
                    p++;
                }
                while (*p == ' ' || *p == '\t') p++;
                char *amt_s = p;
                while (*p && *p != ' ' && *p != '\t') p++;
                if (*p) {
                    *p = '\0';
                    p++;
                }
                if (*acc != '\0' && *amt_s != '\0') {
                    char *endptr;
                    double amt = strtod(amt_s, &endptr);
                    if (*endptr == '\0' || *endptr == ' ' || *endptr == '\t' || *endptr == '\r' || *endptr == '\n') {
                        int found = 0;
                        for (size_t i = 0; i < count; i++) {
                            if (strcmp(entries[i].account, acc) == 0) {
                                entries[i].balance += amt;
                                found = 1;
                                break;
                            }
                        }
                        if (!found) {
                            if (count >= capacity) {
                                capacity = capacity == 0 ? 4 : capacity * 2;
                                Account *new_entries = realloc(entries, capacity * sizeof(Account));
                                if (new_entries) entries = new_entries;
                            }
                            if (count < capacity) {
                                entries[count].account = strdup(acc);
                                entries[count].balance = amt;
                                count++;
                            }
                        }
                    }
                }
                free(fline);
            }
            fclose(f);
        }
    }
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ' && *p != '\t') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "ADD") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *acc = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *amt_s = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            if (*acc != '\0' && *amt_s != '\0') {
                char *endptr;
                double amt = strtod(amt_s, &endptr);
                if (*endptr == '\0') {
                    int found = 0;
                    for (size_t i = 0; i < count; i++) {
                        if (strcmp(entries[i].account, acc) == 0) {
                            entries[i].balance += amt;
                            found = 1;
                            break;
                        }
                    }
                    if (!found) {
                        if (count >= capacity) {
                            capacity = capacity == 0 ? 4 : capacity * 2;
                            Account *new_entries = realloc(entries, capacity * sizeof(Account));
                            if (new_entries) entries = new_entries;
                        }
                        if (count < capacity) {
                            entries[count].account = strdup(acc);
                            entries[count].balance = amt;
                            count++;
                        }
                    }
                }
            }
        } else if (strcmp(cmd, "REVERSE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *acc = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *amt_s = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            if (*acc != '\0' && *amt_s != '\0') {
                char *endptr;
                double amt = strtod(amt_s, &endptr);
                if (*endptr == '\0') {
                    for (size_t i = 0; i < count; i++) {
                        if (strcmp(entries[i].account, acc) == 0) {
                            entries[i].balance -= amt;
                            break;
                        }
                    }
                }
            }
        } else if (strcmp(cmd, "BALANCE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *acc = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            double bal = 0.0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(entries[i].account, acc) == 0) {
                    bal = entries[i].balance;
                    break;
                }
            }
            printf("%.2lf\n", bal);
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %.2lf\n", entries[i].account, entries[i].balance);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(entries[i].account);
    }
    free(entries);
    return 0;
}
