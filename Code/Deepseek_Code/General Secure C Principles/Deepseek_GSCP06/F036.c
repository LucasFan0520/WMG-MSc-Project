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

Record *find_record(const char *name) {
    Record *r = records;
    while (r) {
        if (strcmp(r->name, name) == 0) return r;
        r = r->next;
    }
    return NULL;
}

void add_record(const char *name, const char *data) {
    if (find_record(name)) return;
    Record *r = malloc(sizeof(Record));
    r->name = strdup(name);
    r->data = strdup(data);
    r->next = records;
    records = r;
}

void delete_record(const char *name) {
    Record *cur = records, *prev = NULL;
    while (cur) {
        if (strcmp(cur->name, name) == 0) {
            if (prev) prev->next = cur->next;
            else records = cur->next;
            free(cur->name);
            free(cur->data);
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void show_record(const char *name) {
    Record *r = find_record(name);
    if (!r) return;
    for (char *c = r->data; *c; c++) {
        if (*c == ' ') putchar('_');
        else putchar(*c);
    }
    putchar('\n');
}

void report_names(void) {
    Record *r = records;
    while (r) {
        printf("%s\n", r->name);
        r = r->next;
    }
}

void cleanup_records(void) {
    Record *r = records;
    while (r) {
        Record *tmp = r;
        r = r->next;
        free(tmp->name);
        free(tmp->data);
        free(tmp);
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], name[256], rest[4096];
        int n = sscanf(line, "%31s %255s %4095s", cmd, name, rest);
        if (n < 2) continue;
        if (strcmp(cmd, "ADD") == 0 && n >= 3) {
            char *data_str = line + strlen(cmd) + 1 + strlen(name) + 1;
            // parse length:data
            char *colon = strchr(data_str, ':');
            if (!colon) continue;
            *colon = 0;
            int len = atoi(data_str);
            char *data = colon + 1;
            if (len < 0 || (int)strlen(data) != len) continue;
            add_record(name, data);
        } else if (strcmp(cmd, "DELETE") == 0) {
            delete_record(name);
        } else if (strcmp(cmd, "SHOW") == 0) {
            show_record(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_names();
        }
    }
    cleanup_records();
    return 0;
}