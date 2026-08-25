/* F029.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char *value;
} Record;

static Record *records = NULL;
static int rcount = 0;
static int rcap = 0;

static char *dup_str(const char *s) {
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (d) memcpy(d, s, len + 1);
    return d;
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') line[len - 1] = '\0';
        if (strcmp(line, "END") == 0) break;

        char cmd[64] = {0};
        int offset = 0;
        if (sscanf(line, "%63s%n", cmd, &offset) != 1) continue;

        if (strcmp(cmd, "ADD") == 0) {
            char key[256] = {0}, val[256] = {0};
            if (sscanf(line + offset, "%255s %255s", key, val) != 2) continue;
            if (rcount >= rcap) {
                rcap = rcap == 0 ? 8 : rcap * 2;
                records = realloc(records, sizeof(Record) * rcap);
            }
            records[rcount].key = dup_str(key);
            records[rcount].value = dup_str(val);
            rcount++;
        } else if (strcmp(cmd, "DEDUP") == 0) {
            int i = 0;
            while (i < rcount) {
                int dup = 0;
                for (int j = 0; j < i; j++) {
                    if (strcmp(records[i].key, records[j].key) == 0) {
                        dup = 1;
                        break;
                    }
                }
                if (dup) {
                    free(records[i].key);
                    free(records[i].value);
                    for (int j = i; j < rcount - 1; j++) records[j] = records[j + 1];
                    rcount--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char key[256] = {0};
            if (sscanf(line + offset, "%255s", key) != 1) continue;
            int i = 0;
            while (i < rcount) {
                if (strcmp(records[i].key, key) == 0) {
                    free(records[i].key);
                    free(records[i].value);
                    for (int j = i; j < rcount - 1; j++) records[j] = records[j + 1];
                    rcount--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char key[256] = {0};
            if (sscanf(line + offset, "%255s", key) != 1) continue;
            for (int i = 0; i < rcount; i++) {
                if (strcmp(records[i].key, key) == 0) {
                    printf("%s\n", records[i].value);
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < rcount; i++) {
                printf("%s %s\n", records[i].key, records[i].value);
            }
        }
    }
    for (int i = 0; i < rcount; i++) {
        free(records[i].key);
        free(records[i].value);
    }
    free(records);
    return 0;
}