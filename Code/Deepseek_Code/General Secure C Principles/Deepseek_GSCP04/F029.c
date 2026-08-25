/* F029.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Record {
    char *key;
    char *value;
} Record;

static Record *records = NULL;
static size_t rcount = 0, rcap = 0;

static void add_record(const char *key, const char *value) {
    if (rcount == rcap) {
        rcap = rcap ? rcap * 2 : 4;
        records = realloc(records, rcap * sizeof(Record));
        if (!records) { perror("realloc"); exit(1); }
    }
    records[rcount].key = strdup(key);
    records[rcount].value = strdup(value);
    rcount++;
}

static void dedup(void) {
    for (size_t i = 0; i < rcount; i++) {
        for (size_t j = i + 1; j < rcount; ) {
            if (strcmp(records[i].key, records[j].key) == 0) {
                free(records[j].key);
                free(records[j].value);
                for (size_t k = j; k < rcount - 1; k++)
                    records[k] = records[k + 1];
                rcount--;
            } else {
                j++;
            }
        }
    }
}

static void delete_key(const char *key) {
    for (size_t i = 0; i < rcount; ) {
        if (strcmp(records[i].key, key) == 0) {
            free(records[i].key);
            free(records[i].value);
            for (size_t j = i; j < rcount - 1; j++)
                records[j] = records[j + 1];
            rcount--;
        } else {
            i++;
        }
    }
}

static void find_key(const char *key) {
    for (size_t i = 0; i < rcount; i++) {
        if (strcmp(records[i].key, key) == 0) {
            printf("%s\n", records[i].value);
        }
    }
}

static void report_records(void) {
    for (size_t i = 0; i < rcount; i++) {
        printf("%s %s\n", records[i].key, records[i].value);
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *key = strtok(NULL, " ");
            if (!key) continue;
            char *val = strtok(NULL, "");
            if (!val) val = "";
            add_record(key, val);
        } else if (strcmp(cmd, "DEDUP") == 0) {
            dedup();
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *key = strtok(NULL, " ");
            if (key) delete_key(key);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *key = strtok(NULL, " ");
            if (key) find_key(key);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_records();
        }
    }
    for (size_t i = 0; i < rcount; i++) {
        free(records[i].key);
        free(records[i].value);
    }
    free(records);
    return 0;
}