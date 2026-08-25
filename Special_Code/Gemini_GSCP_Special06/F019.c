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
    return buf;
}

char *mystrdup(const char *s) {
    size_t l = strlen(s);
    char *d = malloc(l + 1);
    if (!d) return NULL;
    strcpy(d, s);
    return d;
}

typedef struct {
    char *account;
    double balance;
} Account;

int main(int argc, char **argv) {
    Account *arr = NULL;
    int count = 0;
    int cap = 0;
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char *fline;
            while ((fline = read_line(f)) != NULL) {
                char *p = fline;
                while (*p == ' ' || *p == '	') p++;
                if (*p == '\0') { free(fline); continue; }
                char *account = p;
                while (*p && *p != ' ' && *p != '	') p++;
                if (*p) { *p = '\0'; p++; }
                while (*p == ' ' || *p == '	') p++;
                char *amt_s = p;
                while (*p && *p != ' ' && *p != '	') p++;
                if (*p) { *p = '\0'; p++; }
                char *endptr;
                double amt = strtod(amt_s, &endptr);
                if (endptr != amt_s) {
                    int found = 0;
                    for (int i = 0; i < count; i++) {
                        if (strcmp(arr[i].account, account) == 0) {
                            arr[i].balance += amt;
                            found = 1;
                            break;
                        }
                    }
                    if (!found) {
                        if (count >= cap) {
                            cap = cap == 0 ? 8 : cap * 2;
                            Account *narr = realloc(arr, cap * sizeof(Account));
                            if (narr) arr = narr;
                        }
                        if (count < cap) {
                            arr[count].account = mystrdup(account);
                            arr[count].balance = amt;
                            count++;
                        }
                    }
                }
                free(fline);
            }
            fclose(f);
        }
    }
    char *line;
    while ((line = read_line(stdin)) != NULL) {
        char *p = line;
        while (*p == ' ' || *p == '	' || *p == '\n' || *p == '\n') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ' && *p != '	' && *p != '\n' && *p != '\n') p++;
        if (*p) { *p = '\0'; p++; }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "ADD") == 0) {
            while (*p == ' ' || *p == '	') p++;
            char *account = p;
            while (*p && *p != ' ' && *p != '	' && *p != '\n' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ' || *p == '	') p++;
            char *amt_s = p;
            while (*p && *p != ' ' && *p != '	' && *p != '\n' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            char *endptr;
            double amt = strtod(amt_s, &endptr);
            if (endptr != amt_s) {
                int found = 0;
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].account, account) == 0) {
                        arr[i].balance += amt;
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    if (count >= cap) {
                        cap = cap == 0 ? 8 : cap * 2;
                        Account *narr = realloc(arr, cap * sizeof(Account));
                        if (narr) arr = narr;
                    }
                    if (count < cap) {
                        arr[count].account = mystrdup(account);
                        arr[count].balance = amt;
                        count++;
                    }
                }
            }
        } else if (strcmp(cmd, "REVERSE") == 0) {
            while (*p == ' ' || *p == '	') p++;
            char *account = p;
            while (*p && *p != ' ' && *p != '	' && *p != '\n' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ' || *p == '	') p++;
            char *amt_s = p;
            while (*p && *p != ' ' && *p != '	' && *p != '\n' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            char *endptr;
            double amt = strtod(amt_s, &endptr);
            if (endptr != amt_s) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].account, account) == 0) {
                        arr[i].balance -= amt;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "BALANCE") == 0) {
            while (*p == ' ' || *p == '	') p++;
            char *account = p;
            size_t al = strlen(account);
            while (al > 0 && (account[al-1] == '\n' || account[al-1] == '\n')) {
                account[al-1] = '\0';
                al--;
            }
            double bal = 0;
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].account, account) == 0) {
                    bal = arr[i].balance;
                    break;
                }
            }
            printf("%.2f\n", bal);
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %.2f\n", arr[i].account, arr[i].balance);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(arr[i].account);
    }
    free(arr);
    return 0;
}
