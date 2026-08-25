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
    Record *prev = NULL;
    Record *r = records;
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
    for (char *s = r->data; *s; s++) putchar(*s == ' ' ? '_' : *s);
    putchar('\n');
}

void report_names() {
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

void free_all() {
    Record *r = records;
    while (r) {
        Record *next = r->next;
        free(r->name);
        free(r->data);
        free(r);
        r = next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        char rest[4096];
        char *p = line + strlen(cmd);
        while (*p == ' ') p++;
        strcpy(rest, p);
        if (strcmp(cmd, "ADD") == 0) {
            char name[256];
            char *colon = strchr(rest, ':');
            if (!colon) continue;
            *colon = '\0';
            strcpy(name, rest);
            char *len_str = colon + 1;
            char *data = strchr(len_str, ':');
            if (!data) continue;
            *data = '\0';
            int len = atoi(len_str);
            char *data_start = data + 1;
            if ((int)strlen(data_start) != len) continue;
            add_record(name, data_start);
        } else if (strcmp(cmd, "DELETE") == 0) {
            delete_record(rest);
        } else if (strcmp(cmd, "SHOW") == 0) {
            show_record(rest);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_names();
        }
    }
    free_all();
    return 0;
}