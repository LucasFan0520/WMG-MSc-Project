// F019.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *account;
    long long balance;
} AccountInfo;

AccountInfo *ledger = NULL;
int count = 0;

void update_balance(const char *acct, long long amt) {
    for (int i = 0; i < count; i++) {
        if (strcmp(ledger[i].account, acct) == 0) {
            ledger[i].balance += amt;
            return;
        }
    }
    ledger = realloc(ledger, (count + 1) * sizeof(AccountInfo));
    if (!ledger) exit(1);
    ledger[count].account = strdup(acct);
    ledger[count].balance = amt;
    count++;
}

int main(int argc, char **argv) {
    if (argc >= 2) {
        FILE *fp = fopen(argv[1], "r");
        if (fp) {
            char *acct, *memo;
            char amt_str[128];
            while (fscanf(fp, "%ms %127s %ms", &acct, amt_str, &memo) == 3) {
                char *endptr;
                long long amt = strtoll(amt_str, &endptr, 10);
                if (*endptr == '\0') {
                    update_balance(acct, amt);
                }
                free(acct);
                free(memo);
            }
            fclose(fp);
        }
    }

    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, "END") == 0) {
            break;
        }

        char *cmd = line;
        char *space1 = strchr(line, ' ');
        char *args = NULL;
        if (space1) {
            *space1 = '\0';
            args = space1 + 1;
            while (*args == ' ') args++;
        }

        if (strcmp(cmd, "ADD") == 0 && args) {
            char *acct = args;
            char *space2 = strchr(args, ' ');
            if (space2) {
                *space2 = '\0';
                char *amt_str = space2 + 1;
                while (*amt_str == ' ') amt_str++;
                char *space3 = strchr(amt_str, ' ');
                if (space3) {
                    *space3 = '\0';
                }
                char *endptr;
                long long amt = strtoll(amt_str, &endptr, 10);
                if (*endptr == '\0') {
                    update_balance(acct, amt);
                }
            }
        } else if (strcmp(cmd, "REVERSE") == 0 && args) {
            char *acct = args;
            char *space2 = strchr(args, ' ');
            if (space2) {
                *space2 = '\0';
                char *amt_str = space2 + 1;
                while (*amt_str == ' ') amt_str++;
                char *endptr;
                long long amt = strtoll(amt_str, &endptr, 10);
                if (*endptr == '\0') {
                    update_balance(acct, -amt);
                }
            }
        } else if (strcmp(cmd, "BALANCE") == 0 && args) {
            long long bal = 0;
            for (int i = 0; i < count; i++) {
                if (strcmp(ledger[i].account, args) == 0) {
                    bal = ledger[i].balance;
                    break;
                }
            }
            printf("%lld\n", bal);
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %lld\n", ledger[i].account, ledger[i].balance);
            }
        }
    }

    free(line);
    for (int i = 0; i < count; i++) {
        free(ledger[i].account);
    }
    free(ledger);
    return 0;
}
