// F029.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *key; char *value; } Record;
Record *recs = NULL;
int rc = 0, rcap = 0;

int find_first_record(const char *key) {
    for (int i = 0; i < rc; i++)
        if (strcmp(recs[i].key, key) == 0) return i;
    return -1;
}

void add_record(const char *key, const char *value) {
    if (rc == rcap) { rcap = rcap ? rcap * 2 : 4; recs = realloc(recs, sizeof(Record) * rcap); }
    recs[rc].key = strdup(key);
    recs[rc].value = strdup(value);
    rc++;
}

void dedup(void) {
    for (int i = 0; i < rc; i++) {
        for (int j = i + 1; j < rc; j++) {
            if (strcmp(recs[i].key, recs[j].key) == 0) {
                free(recs[j].key);
                free(recs[j].value);
                for (int k = j; k < rc - 1; k++) recs[k] = recs[k + 1];
                rc--;
                j--;
            }
        }
    }
}

void delete_key(const char *key) {
    int i = 0;
    while (i < rc) {
        if (strcmp(recs[i].key, key) == 0) {
            free(recs[i].key);
            free(recs[i].value);
            for (int j = i; j < rc - 1; j++) recs[j] = recs[j + 1];
            rc--;
        } else i++;
    }
}

void find_key(const char *key) {
    for (int i = 0; i < rc; i++)
        if (strcmp(recs[i].key, key) == 0) printf("%s\n", recs[i].value);
}

void report_records(void) {
    for (int i = 0; i < rc; i++)
        printf("%s %s\n", recs[i].key, recs[i].value);
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32], arg1[256], arg2[4096];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%*s %255s %4095[^\n]", arg1, arg2) == 2)
                add_record(arg1, arg2);
        } else if (strcmp(cmd, "DEDUP") == 0) dedup();
        else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", arg1) == 1) delete_key(arg1);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(line, "%*s %255s", arg1) == 1) find_key(arg1);
        } else if (strcmp(cmd, "REPORT") == 0) report_records();
    }
    for (int i = 0; i < rc; i++) { free(recs[i].key); free(recs[i].value); }
    free(recs);
    return 0;
}