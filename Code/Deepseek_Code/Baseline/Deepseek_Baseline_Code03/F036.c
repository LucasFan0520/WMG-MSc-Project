// F036.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct record {
    char *name;
    char *data;
    struct record *next;
};

struct record *records = NULL;

struct record *find_record(char *name) {
    struct record *r = records;
    while (r) {
        if (strcmp(r->name, name) == 0) return r;
        r = r->next;
    }
    return NULL;
}

void add_record(char *name, char *data) {
    struct record *r = find_record(name);
    if (r) {
        free(r->data);
        r->data = malloc(strlen(data) + 1);
        strcpy(r->data, data);
    } else {
        r = malloc(sizeof(struct record));
        r->name = malloc(strlen(name) + 1);
        strcpy(r->name, name);
        r->data = malloc(strlen(data) + 1);
        strcpy(r->data, data);
        r->next = records;
        records = r;
    }
}

void delete_record(char *name) {
    struct record *r = records, *prev = NULL;
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

void show_record(char *name) {
    struct record *r = find_record(name);
    if (r) {
        for (char *c = r->data; *c; c++) {
            if (*c == ' ') putchar('_');
            else putchar(*c);
        }
        putchar('\n');
    }
}

void report_names() {
    struct record *r = records;
    while (r) {
        printf("%s\n", r->name);
        r = r->next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char name[256];
        char rest[4096];
        int n = sscanf(line, "%31s %255s %4095[^\n]", cmd, name, rest);
        if (strcmp(cmd, "ADD") == 0 && n >= 2) {
            char *colon = strchr(rest, ':');
            if (!colon) continue;
            *colon = 0;
            int len = atoi(rest);
            char *data = colon + 1;
            if (len != strlen(data)) continue;
            add_record(name, data);
        } else if (strcmp(cmd, "DELETE") == 0 && n >= 2) {
            delete_record(name);
        } else if (strcmp(cmd, "SHOW") == 0 && n >= 2) {
            show_record(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_names();
        }
    }
    return 0;
}