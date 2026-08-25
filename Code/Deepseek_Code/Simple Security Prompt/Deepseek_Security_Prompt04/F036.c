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

Record *head = NULL;

Record *find_record(char *name) {
    for (Record *r = head; r; r = r->next)
        if (strcmp(r->name, name) == 0) return r;
    return NULL;
}

void add_record(char *name, char *data) {
    Record *r = find_record(name);
    if (r) {
        free(r->data);
        r->data = strdup(data);
    } else {
        Record *new = malloc(sizeof(Record));
        new->name = strdup(name);
        new->data = strdup(data);
        new->next = head;
        head = new;
    }
}

void delete_record(char *name) {
    Record *prev = NULL;
    for (Record *r = head; r; r = r->next) {
        if (strcmp(r->name, name) == 0) {
            if (prev) prev->next = r->next;
            else head = r->next;
            free(r->name);
            free(r->data);
            free(r);
            return;
        }
        prev = r;
    }
}

void show_record(char *name) {
    Record *r = find_record(name);
    if (r) {
        for (char *c = r->data; *c; c++) putchar(*c == ' ' ? '_' : *c);
        putchar('\n');
    }
}

void report_names(void) {
    for (Record *r = head; r; r = r->next)
        printf("%s\n", r->name);
}

void free_all(void) {
    Record *r = head;
    while (r) {
        Record *next = r->next;
        free(r->name);
        free(r->data);
        free(r);
        r = next;
    }
}

int main() {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], arg1[256], arg2[1024];
        int n = sscanf(line, "%15s %255s %[^\n]", cmd, arg1, arg2);
        if (n < 1) continue;
        if (strcmp(cmd, "ADD") == 0 && n == 3) {
            // parse arg2 as length:data
            char *colon = strchr(arg2, ':');
            if (colon) {
                *colon = '\0';
                int len = atoi(arg1);
                char *data = colon + 1;
                if (strlen(data) == len) {
                    add_record(arg1, data);
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0 && n == 2) {
            delete_record(arg1);
        } else if (strcmp(cmd, "SHOW") == 0 && n == 2) {
            show_record(arg1);
        } else if (strcmp(cmd, "REPORT") == 0 && n == 1) {
            report_names();
        }
    }
    free(line);
    free_all();
    return 0;
}