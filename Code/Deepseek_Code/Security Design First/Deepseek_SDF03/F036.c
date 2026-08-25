// F036.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096
#define MAX_NAME 64
#define MAX_DATA 4096

typedef struct Record {
    char name[MAX_NAME];
    char data[MAX_DATA];
    struct Record *next;
} Record;

Record *head = NULL;

Record *find_record(const char *name) {
    Record *r = head;
    while (r) {
        if (strcmp(r->name, name) == 0) return r;
        r = r->next;
    }
    return NULL;
}

void free_all() {
    Record *r = head;
    while (r) {
        Record *tmp = r;
        r = r->next;
        free(tmp);
    }
    head = NULL;
}

void handle_add(char *line) {
    char name[MAX_NAME];
    char data[MAX_DATA];
    int len;
    char *colon = strchr(line, ':');
    if (!colon) return;
    if (sscanf(line, "ADD %63s %d:", name, &len) != 2) return;
    char *data_start = colon + 1;
    int actual_len = strlen(data_start);
    if (len != actual_len) return;
    if (len >= MAX_DATA) return;
    strcpy(data, data_start);
    if (find_record(name)) return;
    Record *r = malloc(sizeof(Record));
    if (!r) return;
    strcpy(r->name, name);
    strcpy(r->data, data);
    r->next = head;
    head = r;
}

void handle_delete(char *line) {
    char name[MAX_NAME];
    if (sscanf(line, "DELETE %63s", name) != 1) return;
    Record *r = head, *prev = NULL;
    while (r) {
        if (strcmp(r->name, name) == 0) {
            if (prev) prev->next = r->next;
            else head = r->next;
            free(r);
            return;
        }
        prev = r;
        r = r->next;
    }
}

void handle_show(char *line) {
    char name[MAX_NAME];
    if (sscanf(line, "SHOW %63s", name) != 1) return;
    Record *r = find_record(name);
    if (!r) return;
    for (char *c = r->data; *c; c++) {
        if (*c == ' ') putchar('_');
        else putchar(*c);
    }
    putchar('\n');
}

void handle_report() {
    Record *r = head;
    while (r) {
        printf("%s\n", r->name);
        r = r->next;
    }
}

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        if (strncmp(line, "ADD ", 4) == 0) handle_add(line);
        else if (strncmp(line, "DELETE ", 7) == 0) handle_delete(line);
        else if (strncmp(line, "SHOW ", 5) == 0) handle_show(line);
        else if (strcmp(line, "REPORT") == 0) handle_report();
    }
    free_all();
    return 0;
}