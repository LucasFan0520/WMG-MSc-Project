// F019.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    int cap = 16;
    int len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            char *nb = realloc(buf, cap);
            if (!nb) { free(buf); return NULL; }
            buf = nb;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) { free(buf); return NULL; }
    buf[len] = '\0';
    return buf;
}

char *get_token(char **p) {
    while (**p == ' ') (*p)++;
    if (**p == '\0') return NULL;
    char *start = *p;
    while (**p && **p != ' ') (*p)++;
    if (**p) {
        **p = '\0';
        (*p)++;
    }
    return start;
}

typedef struct {
    char *acc;
    double bal;
} Account;

int main(int argc, char **argv) {
    if (argc < 2) return 1;
    Account *arr = NULL;
    int count = 0;
    int cap = 0;
    FILE *f = fopen(argv[1], "r");
    if (f) {
        while (1) {
            char *line = read_line(f);
            if (!line) break;
            char *p = line;
            char *acc = get_token(&p);
            char *amt_s = get_token(&p);
            if (acc && amt_s) {
                char *endptr;
                double val = strtod(amt_s, &endptr);
                if (endptr != amt_s) {
                    int found = 0;
                    for (int i = 0; i < count; i++) {
                        if (strcmp(arr[i].acc, acc) == 0) {
                            arr[i].bal += val;
                            found = 1;
                            break;
                        }
                    }
                    if (!found) {
                        if (count >= cap) {
                            cap = cap == 0 ? 4 : cap * 2;
                            Account *narr = realloc(arr, cap * sizeof(Account));
                            if (narr) arr = narr;
                        }
                        arr[count].acc = strdup(acc);
                        arr[count].bal = val;
                        count++;
                    }
                }
            }
            free(line);
        }
        fclose(f);
    }
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        char *cmd = get_token(&p);
        if (!cmd || strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "ADD") == 0) {
            char *acc = get_token(&p);
            char *amt_s = get_token(&p);
            if (acc && amt_s) {
                char *endptr;
                double val = strtod(amt_s, &endptr);
                if (endptr != amt_s) {
                    int found = 0;
                    for (int i = 0; i < count; i++) {
                        if (strcmp(arr[i].acc, acc) == 0) {
                            arr[i].bal += val;
                            found = 1;
                            break;
                        }
                    }
                    if (!found) {
                        if (count >= cap) {
                            cap = cap == 0 ? 4 : cap * 2;
                            Account *narr = realloc(arr, cap * sizeof(Account));
                            if (narr) arr = narr;
                        }
                        arr[count].acc = strdup(acc);
                        arr[count].bal = val;
                        count++;
                    }
                }
            }
        } else if (strcmp(cmd, "REVERSE") == 0) {
            char *acc = get_token(&p);
            char *amt_s = get_token(&p);
            if (acc && amt_s) {
                char *endptr;
                double val = strtod(amt_s, &endptr);
                if (endptr != amt_s) {
                    for (int i = 0; i < count; i++) {
                        if (strcmp(arr[i].acc, acc) == 0) {
                            arr[i].bal -= val;
                            break;
                        }
                    }
                }
            }
        } else if (strcmp(cmd, "BALANCE") == 0) {
            char *acc = get_token(&p);
            if (acc) {
                double b = 0.0;
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].acc, acc) == 0) {
                        b = arr[i].bal;
                        break;
                    }
                }
                printf("%.2f\n", b);
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %.2f\n", arr[i].acc, arr[i].bal);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(arr[i].acc);
    }
    free(arr);
    return 0;
}
