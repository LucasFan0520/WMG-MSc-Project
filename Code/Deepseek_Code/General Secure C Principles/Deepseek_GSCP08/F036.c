// F036.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct Record { char *name; char *data; struct Record *next; } Record;
Record *records = NULL;

Record *find_record(const char *name) {
    for (Record *r = records; r; r = r->next)
        if (strcmp(r->name, name) == 0) return r;
    return NULL;
}

void add_record(const char *name, const char *data) {
    if (find_record(name)) return;
    Record *r = malloc(sizeof(Record));
    if (!r) return;
    r->name = strdup(name);
    r->data = strdup(data);
    r->next = records;
    records = r;
}

void delete_record(const char *name) {
    Record *prev = NULL;
    for (Record *r = records; r; prev = r, r = r->next) {
        if (strcmp(r->name, name) == 0) {
            if (prev) prev->next = r->next; else records = r->next;
            free(r->name); free(r->data); free(r);
            return;
        }
    }
}

void show_record(const char *name) {
    Record *r = find_record(name);
    if (!r) { printf("NOT_FOUND\n"); return; }
    for (char *c = r->data; *c; c++) putchar(*c == ' ' ? '_' : *c);
    putchar('\n');
}

void report_names(void) {
    for (Record *r = records; r; r = r->next)
        printf("%s\n", r->name);
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) != 1) continue;
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "ADD") == 0) {
            char name[256], data[4096];
            if (sscanf(rest, "%255s", name) == 1) {
                char *d = rest + strlen(name);
                while (*d == ' ') d++;
                // parse length:data
                int len = 0;
                while (isdigit(*d)) { len = len * 10 + (*d - '0'); d++; }
                if (*d != ':') continue;
                d++;
                if ((int)strlen(d) != len) continue;
                strcpy(data, d);
                add_record(name, data);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char name[256];
            if (sscanf(rest, "%255s", name) == 1) delete_record(name);
        } else if (strcmp(cmd, "SHOW") == 0) {
            char name[256];
            if (sscanf(rest, "%255s", name) == 1) show_record(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_names();
        }
    }
    while (records) { Record *r = records; records = r->next; free(r->name); free(r->data); free(r); }
    return 0;
}