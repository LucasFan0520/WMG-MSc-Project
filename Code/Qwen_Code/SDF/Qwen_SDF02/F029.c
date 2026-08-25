// F029.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 4096

typedef struct Record {
    char *key;
    char *value;
    struct Record *next;
} Record;

Record *records = NULL;

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        
        if (strcmp(cmd, "ADD") == 0) {
            char key[256], val[256];
            if (sscanf(rest, "%255s %255s", key, val) != 2) continue;
            Record *r = malloc(sizeof(Record));
            if (!r) continue;
            r->key = strdup(key);
            r->value = strdup(val);
            if (!r->key || !r->value) { free(r->key); free(r->value); free(r); continue; }
            r->next = records;
            records = r;
        } else if (strcmp(cmd, "DEDUP") == 0) {
            Record *prev = NULL;
            for (Record *r = records; r; ) {
                int dup = 0;
                for (Record *c = records; c != r; c = c->next) {
                    if (strcmp(c->key, r->key) == 0) { dup = 1; break; }
                }
                if (dup) {
                    Record *to_del = r;
                    r = r->next;
                    if (prev) prev->next = r;
                    else records = r;
                    free(to_del->key);
                    free(to_del->value);
                    free(to_del);
                } else {
                    prev = r;
                    r = r->next;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char key[256];
            if (sscanf(rest, "%255s", key) != 1) continue;
            Record *prev = NULL;
            for (Record *r = records; r; ) {
                if (strcmp(r->key, key) == 0) {
                    Record *to_del = r;
                    r = r->next;
                    if (prev) prev->next = r;
                    else records = r;
                    free(to_del->key);
                    free(to_del->value);
                    free(to_del);
                } else {
                    prev = r;
                    r = r->next;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char key[256];
            if (sscanf(rest, "%255s", key) != 1) continue;
            for (Record *r = records; r; r = r->next) {
                if (strcmp(r->key, key) == 0) printf("%s\n", r->value);
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (Record *r = records; r; r = r->next) {
                printf("%s %s\n", r->key, r->value);
            }
        }
    }
    while (records) {
        Record *r = records;
        records = records->next;
        free(r->key);
        free(r->value);
        free(r);
    }
    return 0;
}