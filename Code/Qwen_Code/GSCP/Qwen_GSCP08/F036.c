// F036.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Record {
    char *name;
    char *data;
    struct Record *next;
} Record;

Record *head = NULL;

Record *find_record(char *name) {
    for (Record *r = head; r; r = r->next) {
        if (strcmp(r->name, name) == 0) return r;
    }
    return NULL;
}

void add_record(char *name, char *raw_data) {
    char *colon = strchr(raw_data, ':');
    if (!colon) return;
    *colon = 0;
    int len = atoi(raw_data);
    if (len < 0 || len > (int)strlen(colon + 1)) return;
    
    char *data = malloc(len + 1);
    strncpy(data, colon + 1, len);
    data[len] = 0;
    
    Record *r = find_record(name);
    if (!r) {
        r = malloc(sizeof(Record));
        r->name = strdup(name);
        r->next = head;
        head = r;
    } else {
        free(r->data);
    }
    r->data = data;
}

void delete_record(char *name) {
    Record *prev = NULL;
    Record *curr = head;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            free(curr->name);
            free(curr->data);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void show_record(char *name) {
    Record *r = find_record(name);
    if (!r) return;
    for (char *c = r->data; *c; c++) {
        putchar(*c == ' ' ? '_' : *c);
    }
    putchar('\n');
}

void report_records() {
    for (Record *r = head; r; r = r->next) {
        printf("%s\n", r->name);
    }
}

int main() {
    char cmd[20], a[100], raw[2000];
    while (scanf("%19s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "ADD") == 0) {
            scanf("%99s", a);
            fgets(raw, sizeof(raw), stdin);
            raw[strcspn(raw, "\n")] = 0;
            add_record(a, raw);
        } else if (strcmp(cmd, "DELETE") == 0) {
            scanf("%99s", a);
            delete_record(a);
        } else if (strcmp(cmd, "SHOW") == 0) {
            scanf("%99s", a);
            show_record(a);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_records();
        }
    }
    return 0;
}