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
    Record *r = records, *prev = NULL;
    while (r && strcmp(r->name, name) != 0) {
        prev = r;
        r = r->next;
    }
    if (!r) return;
    if (prev) prev->next = r->next;
    else records = r->next;
    free(r->name);
    free(r->data);
    free(r);
}

void show_record(const char *name) {
    Record *r = find_record(name);
    if (!r) { printf("NOT_FOUND\n"); return; }
    for (char *c = r->data; *c; c++) {
        putchar(*c == ' ' ? '_' : *c);
    }
    putchar('\n');
}

void report_names(void) {
    Record *r = records;
    int first = 1;
    while (r) {
        if (!first) putchar(' ');
        printf("%s", r->name);
        first = 0;
        r = r->next;
    }
    putchar('\n');
}

void free_all(void) {
    Record *r = records;
    while (r) {
        Record *next = r->next;
        free(r->name);
        free(r->data);
        free(r);
        r = next;
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char name[256];
            char *p = line + 4;
            if (sscanf(p, "%255s", name) != 1) continue;
            while (*p && *p != ' ') p++;
            while (*p == ' ') p++;
            char *colon = strchr(p, ':');
            if (!colon) continue;
            *colon = '\0';
            char *len_str = p;
            char *data = colon + 1;
            int len_val = atoi(len_str);
            if (len_val < 0) continue;
            if ((int)strlen(data) != len_val) continue;
            add_record(name, data);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            delete_record(p);
        } else if (strcmp(cmd, "SHOW") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            show_record(p);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_names();
        }
    }
    free_all();
    free(line);
    return 0;
}