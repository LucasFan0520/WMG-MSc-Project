// F036.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Record {
    char *name;
    char *data;
    struct Record *next;
} Record;

Record *records = NULL;

void add_record(const char *name, const char *raw) {
    char *colon = strchr(raw, ':');
    if (!colon) return;
    int len = 0;
    if (sscanf(raw, "%d", &len) != 1 || len < 0) return;
    if (strlen(colon + 1) != (size_t)len) return;
    
    Record *r = malloc(sizeof(Record));
    r->name = strdup(name);
    r->data = strdup(colon + 1);
    r->next = records;
    records = r;
}

void delete_record(const char *name) {
    Record **rp = &records;
    while (*rp) {
        if (strcmp((*rp)->name, name) == 0) {
            Record *tmp = *rp;
            *rp = tmp->next;
            free(tmp->name);
            free(tmp->data);
            free(tmp);
            return;
        }
        rp = &(*rp)->next;
    }
}

void show_record(const char *name) {
    for (Record *r = records; r; r = r->next) {
        if (strcmp(r->name, name) == 0) {
            for (char *c = r->data; *c; c++) {
                putchar(*c == ' ' ? '_' : *c);
            }
            putchar('\n');
            return;
        }
    }
}

void report() {
    for (Record *r = records; r; r = r->next) {
        printf("%s\n", r->name);
    }
}

void cleanup() {
    while (records) {
        Record *tmp = records->next;
        free(records->name);
        free(records->data);
        free(records);
        records = tmp;
    }
}

int main() {
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], a1[256];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%*s %255s", a1) == 1) {
                char *t = strchr(line, ' ');
                if (t) { t = strchr(t + 1, ' '); if (t) add_record(a1, t + 1); }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", a1) == 1) delete_record(a1);
        } else if (strcmp(cmd, "SHOW") == 0) {
            if (sscanf(line, "%*s %255s", a1) == 1) show_record(a1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report();
        }
    }
    cleanup();
    return 0;
}