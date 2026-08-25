// F019.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *account;
    double balance;
} AccountInfo;

int main(int argc, char **argv) {
    if (argc < 2) {
        return 1;
    }
    AccountInfo *list = NULL;
    int count = 0;
    FILE *fp = fopen(argv[1], "r");
    char *line = NULL;
    size_t cap = 0;
    ssize_t read;
    if (fp) {
        while ((read = getline(&line, &cap, fp)) != -1) {
            line[strcspn(line, "\r\n")] = 0;
            char acc[256], memo[256];
            double amt;
            if (sscanf(line, "%255s %lf %255s", acc, &amt, memo) == 3) {
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(list[i].account, acc) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    list[found].balance += amt;
                } else {
                    AccountInfo *tmp = realloc(list, (count + 1) * sizeof(AccountInfo));
                    if (tmp) {
                        list = tmp;
                        list[count].account = strdup(acc);
                        list[count].balance = amt;
                        count++;
                    }
                }
            }
        }
        fclose(fp);
    }
    while ((read = getline(&line, &cap, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char acc[256], memo[256];
            double amt;
            if (sscanf(line + 4, "%255s %lf %255s", acc, &amt, memo) == 3) {
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(list[i].account, acc) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    list[found].balance += amt;
                } else {
                    AccountInfo *tmp = realloc(list, (count + 1) * sizeof(AccountInfo));
                    if (tmp) {
                        list = tmp;
                        list[count].account = strdup(acc);
                        list[count].balance = amt;
                        count++;
                    }
                }
            }
        } else if (strncmp(line, "REVERSE ", 8) == 0) {
            char acc[256];
            double amt;
            if (sscanf(line + 8, "%255s %lf", acc, &amt) == 2) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(list[i].account, acc) == 0) {
                        list[i].balance -= amt;
                        break;
                    }
                }
            }
        } else if (strncmp(line, "BALANCE ", 8) == 0) {
            char *acc = line + 8;
            while (*acc == ' ') {
                acc++;
            }
            double bal = 0.0;
            for (int i = 0; i < count; i++) {
                if (strcmp(list[i].account, acc) == 0) {
                    bal = list[i].balance;
                    break;
                }
            }
            printf("%.2f\n", bal);
        } else if (strcmp(line, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %.2f\n", list[i].account, list[i].balance);
            }
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(list[i].account);
    }
    free(list);
    return 0;
}
