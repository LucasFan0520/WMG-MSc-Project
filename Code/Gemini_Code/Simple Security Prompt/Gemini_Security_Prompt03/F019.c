// F019.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *account;
    double balance;
} AccountBal;

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

int main(int argc, char **argv) {
    if (argc < 2) return 1;
    AccountBal *list = NULL;
    int count = 0;
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
                if (*p) { *p = '\0'; p++; }
                while (*p == ' ') p++;
                char *amt_s = p;
                while (*p && *p != ' ') p++;
                if (*p) { *p = '\0'; p++; }
                double val = atof(amt_s);
                int found = 0;
                for (int i = 0; i < count; i++) {
                    if (list[i].account && strcmp(list[i].account, acc) == 0) {
                        list[i].balance += val;
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    list = realloc(list, (count + 1) * sizeof(AccountBal));
                    list[count].account = strdup(acc);
                    list[count].balance = val;
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
        char *p = line;
        while (*p == ' ') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p) { *p = '\0'; p++; }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "ADD") == 0 || strcmp(cmd, "REVERSE") == 0) {
            int is_add = (strcmp(cmd, "ADD") == 0);
            while (*p == ' ') p++;
            char *acc = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *amt_s = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            double val = atof(amt_s);
            if (!is_add) val = -val;
            int found = 0;
            for (int i = 0; i < count; i++) {
                if (list[i].account && strcmp(list[i].account, acc) == 0) {
                    list[i].balance += val;
                    found = 1;
                    break;
                }
            }
            if (!found) {
                list = realloc(list, (count + 1) * sizeof(AccountBal));
                list[count].account = strdup(acc);
                list[count].balance = val;
                count++;
            }
        } else if (strcmp(cmd, "BALANCE") == 0) {
            while (*p == ' ') p++;
            char *acc = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            double bal = 0.0;
            for (int i = 0; i < count; i++) {
                if (list[i].account && strcmp(list[i].account, acc) == 0) {
                    bal = list[i].balance;
                    break;
                }
            }
            printf("%.2f\n", bal);
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                if (list[i].account) {
                    printf("%s %.2f\n", list[i].account, list[i].balance);
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        if (list[i].account) free(list[i].account);
    }
    free(list);
    return 0;
}
