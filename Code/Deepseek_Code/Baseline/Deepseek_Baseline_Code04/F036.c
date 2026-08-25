/* F036.c */
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
    Record *prev = NULL, *cur = records;
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
    char *s = strdup(r->data);
    for (char *p = s; *p; p++) if (*p == ' ') *p = '_';
    printf("%s\n", s);
    free(s);
}

void report_names(void) {
    Record *r = records;
    while (r) {
        printf("%s\n", r->name);
        r = r->next;
    }
}

void free_records(void) {
    while (records) {
        Record *tmp = records;
        records = records->next;
        free(tmp->name);
        free(tmp->data);
        free(tmp);
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        char name[256];
        int n = sscanf(line, "%s %s", cmd, name);
        if (strcmp(cmd, "ADD") == 0) {
            if (n < 2) continue;
            char *p = line + strlen(cmd) + 1 + strlen(name) + 1;
            while (*p == ' ') p++;
            // parse length:data
            char *colon = strchr(p, ':');
            if (!colon) continue;
            *colon = '\0';
            int len = atoi(p);
            if (len < 0) continue;
            char *data = colon + 1;
            if (strlen(data) != len) continue;
            add_record(name, data);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (n < 2) continue;
            delete_record(name);
        } else if (strcmp(cmd, "SHOW") == 0) {
            if (n < 2) continue;
            show_record(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_names();
        }
    }
    free_records();
    return 0;
}