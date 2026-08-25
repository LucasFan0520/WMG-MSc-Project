// F029.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_KEY 100
#define MAX_VAL 1000

typedef struct {
    char key[MAX_KEY];
    char value[MAX_VAL];
} Record;

Record records[10000];
int rec_count = 0;

int main() {
    char cmd[20], key[MAX_KEY], value[MAX_VAL];
    while (scanf("%s", cmd) == 1) {
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "ADD") == 0) {
            scanf("%s %s", key, value);
            strcpy(records[rec_count].key, key);
            strcpy(records[rec_count].value, value);
            rec_count++;
        } else if (strcmp(cmd, "DEDUP") == 0) {
            int i = 0;
            while (i < rec_count) {
                int found = 0;
                for (int j = 0; j < i; j++) {
                    if (strcmp(records[i].key, records[j].key) == 0) {
                        found = 1;
                        break;
                    }
                }
                if (found) {
                    for (int j = i; j < rec_count - 1; j++) records[j] = records[j + 1];
                    rec_count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            scanf("%s", key);
            int i = 0;
            while (i < rec_count) {
                if (strcmp(records[i].key, key) == 0) {
                    for (int j = i; j < rec_count - 1; j++) records[j] = records[j + 1];
                    rec_count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            scanf("%s", key);
            for (int i = 0; i < rec_count; i++) {
                if (strcmp(records[i].key, key) == 0) printf("%s\n", records[i].value);
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < rec_count; i++) printf("%s %s\n", records[i].key, records[i].value);
        }
    }
    return 0;
}