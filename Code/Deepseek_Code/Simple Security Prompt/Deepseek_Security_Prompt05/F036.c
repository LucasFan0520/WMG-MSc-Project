/* F036.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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

void add_record(const char *name, const char *spec) {
    if (find_record(name)) return;
    char *colon = strchr(spec, ':');
    if (!colon) return;
    *colon = '\0';
    int len = atoi(spec);
    char *data = colon + 1;
    if (len < 0 || (int)strlen(data) != len) return;
    Record *r = malloc(sizeof(Record));
    if (!r) return;
    r->name = strdup(name);
    r->data = strdup(data);
    r->next = records;
    records = r;
}

void delete_record(const char *name) {
    Record *r = records, *prev = NULL;
    while (r) {
        if (strcmp(r->name, name) == 0) {
            if (prev) prev->next = r->next;
            else records = r->next;
            free(r->name);
            free(r->data);
            free(r);
            return;
        }
        prev = r;
        r = r->next;
    }
}

void show_record(const char *name) {
    Record *r = find_record(name);
    if (!r) {
        printf("NOT_FOUND\n");
        return;
    }
    char *out = strdup(r->data);
    for (char *c = out; *c; c++) if (*c == ' ') *c = '_';
    printf("%s\n", out);
    free(out);
}

void report_names(void) {
    Record *r = records;
    while (r) {
        printf("%s\n", r->name);
        r = r->next;
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], arg1[256], arg2[2048];
        int n = sscanf(line, "%15s %255s %2047s", cmd, arg1, arg2);
        if (strcmp(cmd, "ADD") == 0 && n >= 3) {
            add_record(arg1, arg2);
        } else if (strcmp(cmd, "DELETE") == 0 && n >= 2) {
            delete_record(arg1);
        } else if (strcmp(cmd, "SHOW") == 0 && n >= 2) {
            show_record(arg1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_names();
        }
    }
    while (records) {
        Record *tmp = records;
        records = records->next;
        free(tmp->name);
        free(tmp->data);
        free(tmp);
    }
    return 0;
}