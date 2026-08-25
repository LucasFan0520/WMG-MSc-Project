// F036.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct rec {
    char *name;
    char *data;
    struct rec *next;
} Rec;

Rec *head = NULL;
Rec *tail = NULL;

Rec *find_rec(const char *name) {
    Rec *r = head;
    while (r) {
        if (strcmp(r->name, name) == 0) return r;
        r = r->next;
    }
    return NULL;
}

void add_record(const char *name, const char *data) {
    if (find_rec(name)) return;
    Rec *r = malloc(sizeof(Rec));
    r->name = strdup(name);
    r->data = strdup(data);
    r->next = NULL;
    if (tail) tail->next = r;
    else head = r;
    tail = r;
}

void delete_record(const char *name) {
    Rec *r = head, *prev = NULL;
    while (r) {
        if (strcmp(r->name, name) == 0) {
            if (prev) prev->next = r->next;
            else head = r->next;
            if (r == tail) tail = prev;
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
    Rec *r = find_rec(name);
    if (!r) return;
    for (char *c = r->data; *c; c++)
        putchar(*c == ' ' ? '_' : *c);
    putchar('\n');
}

void report_names(void) {
    Rec *r = head;
    while (r) {
        printf("%s\n", r->name);
        r = r->next;
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        if (strcmp(line, "END\n") == 0 || strcmp(line, "END") == 0) break;
        char cmd[32];
        char name[256], data[4096];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *colon = strchr(line, ':');
            if (!colon) continue;
            int len = 0;
            if (sscanf(line, "%*s %255s %d:", name, &len) == 2) {
                char *start = colon + 1;
                if ((int)strlen(start) >= len) {
                    char data[4096];
                    strncpy(data, start, len);
                    data[len] = '\0';
                    add_record(name, data);
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1)
                delete_record(name);
        } else if (strcmp(cmd, "SHOW") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1)
                show_record(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_names();
        }
    }
    return 0;
}