// F032.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_KEY 100
#define MAX_VAL 1000

typedef struct {
    char key[MAX_KEY];
    char value[MAX_VAL];
} Setting;

Setting committed[1000];
int com_count = 0;

Setting staged[1000];
int stg_count = 0;
int in_transaction = 0;

int find_com(const char *key) {
    for (int i = 0; i < com_count; i++) if (strcmp(committed[i].key, key) == 0) return i;
    return -1;
}

int find_stg(const char *key) {
    for (int i = 0; i < stg_count; i++) if (strcmp(staged[i].key, key) == 0) return i;
    return -1;
}

int main() {
    char cmd[20], key[MAX_KEY], value[MAX_VAL];
    while (scanf("%s", cmd) == 1) {
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "SET") == 0) {
            scanf("%s", key);
            fgets(value, MAX_VAL, stdin);
            if (value[0] == ' ') memmove(value, value + 1, strlen(value));
            if (value[strlen(value) - 1] == '\n') value[strlen(value) - 1] = '\0';
            int idx = find_com(key);
            if (idx == -1) {
                strcpy(committed[com_count].key, key);
                strcpy(committed[com_count].value, value);
                com_count++;
            } else {
                strcpy(committed[idx].value, value);
            }
        } else if (strcmp(cmd, "BEGIN") == 0) {
            in_transaction = 1;
            stg_count = 0;
        } else if (strcmp(cmd, "TSET") == 0) {
            if (in_transaction) {
                scanf("%s", key);
                fgets(value, MAX_VAL, stdin);
                if (value[0] == ' ') memmove(value, value + 1, strlen(value));
                if (value[strlen(value) - 1] == '\n') value[strlen(value) - 1] = '\0';
                int idx = find_stg(key);
                if (idx == -1) {
                    strcpy(staged[stg_count].key, key);
                    strcpy(staged[stg_count].value, value);
                    stg_count++;
                } else {
                    strcpy(staged[idx].value, value);
                }
            }
        } else if (strcmp(cmd, "COMMIT") == 0) {
            if (in_transaction) {
                for (int i = 0; i < stg_count; i++) {
                    int idx = find_com(staged[i].key);
                    if (idx == -1) {
                        strcpy(committed[com_count].key, staged[i].key);
                        strcpy(committed[com_count].value, staged[i].value);
                        com_count++;
                    } else {
                        strcpy(committed[idx].value, staged[i].value);
                    }
                }
                in_transaction = 0;
                stg_count = 0;
            }
        } else if (strcmp(cmd, "ABORT") == 0) {
            in_transaction = 0;
            stg_count = 0;
        } else if (strcmp(cmd, "GET") == 0) {
            scanf("%s", key);
            int idx = find_com(key);
            if (idx != -1) printf("%s\n", committed[idx].value);
        } else if (strcmp(cmd, "DUMP") == 0) {
            for (int i = 0; i < com_count; i++) printf("%s=%s\n", committed[i].key, committed[i].value);
        }
    }
    return 0;
}