// F036.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Record {
    char *name;
    char *data;
    struct Record *next;
} Record;

Record *head = NULL, *tail = NULL;

Record *find_record(const char *name) {
    Record *r = head;
    while (r) {
        if (strcmp(r->name, name) == 0) return r;
        r = r->next;
    }
    return NULL;
}

void add_record(const char *name, const char *length_data) {
    char *colon = strchr(length_data, ':');
    if (!colon) return;
    *colon = '\0';
    int len = atoi(length_data);
    char *data = colon + 1;
    if ((int)strlen(data) != len) return;
    Record *r = malloc(sizeof(Record));
    r->name = malloc(strlen(name) + 1);
    strcpy(r->name, name);
    r->data = malloc(len + 1);
    strcpy(r->data, data);
    r->next = NULL;
    if (tail) {
        tail->next = r;
        tail = r;
    } else {
        head = tail = r;
    }
}

void delete_record(const char *name) {
    Record *prev = NULL, *curr = head;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            if (curr == tail) tail = prev;
            free(curr->name);
            free(curr->data);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void show_record(const char *name) {
    Record *r = find_record(name);
    if (!r) return;
    for (char *c = r->data; *c; c++) {
        putchar(*c == ' ' ? '_' : *c);
    }
    putchar('\n');
}

void report_names(void) {
    Record *r = head;
    while (r) {
        printf("%s\n", r->name);
        r = r->next;
    }
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

int main(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        if (line[read - 1] == '\n') line[read - 1] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *name = strtok(NULL, " ");
            char *ld = strtok(NULL, " ");
            if (name && ld) add_record(name, ld);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = strtok(NULL, " ");
            if (name) delete_record(name);
        } else if (strcmp(cmd, "SHOW") == 0) {
            char *name = strtok(NULL, " ");
            if (name) show_record(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_names();
        }
    }
    free(line);
    free_all();
    return 0;
}