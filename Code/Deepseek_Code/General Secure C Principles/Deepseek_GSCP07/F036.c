// F036.c
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

Record* find_record(const char *name) {
    Record *r = head;
    while (r) {
        if (strcmp(r->name, name) == 0) return r;
        r = r->next;
    }
    return NULL;
}

void free_record(Record *r) {
    free(r->name);
    free(r->data);
    free(r);
}

int parse_length_data(const char *input, int *len, char **data) {
    // format: length:data
    const char *colon = strchr(input, ':');
    if (!colon) return 0;
    // check that everything before colon is digits
    const char *p = input;
    while (p < colon) {
        if (!isdigit(*p)) return 0;
        p++;
    }
    int length = atoi(input);
    if (length < 0) return 0;
    // data starts after colon
    const char *d = colon + 1;
    int actual_len = strlen(d);
    if (actual_len != length) return 0; // must match exactly
    *len = length;
    *data = strdup(d);
    return 1;
}

void handle_add(char *cmd) {
    char *name = strtok(NULL, " ");
    if (!name) return;
    char *rest = strtok(NULL, "");
    if (!rest) return;
    if (find_record(name)) return;
    int len;
    char *data;
    if (!parse_length_data(rest, &len, &data)) return;
    Record *new = malloc(sizeof(Record));
    new->name = strdup(name);
    new->data = data;
    new->next = NULL;
    if (!head) head = new;
    else {
        Record *last = head;
        while (last->next) last = last->next;
        last->next = new;
    }
}

void handle_delete(char *cmd) {
    char *name = strtok(NULL, " ");
    if (!name) return;
    Record *r = head, *prev = NULL;
    while (r) {
        if (strcmp(r->name, name) == 0) {
            if (prev) prev->next = r->next;
            else head = r->next;
            free_record(r);
            return;
        }
        prev = r;
        r = r->next;
    }
}

void handle_show(char *cmd) {
    char *name = strtok(NULL, " ");
    if (!name) return;
    Record *r = find_record(name);
    if (!r) return;
    for (char *s = r->data; *s; s++) {
        putchar(*s == ' ' ? '_' : *s);
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
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "ADD") == 0) handle_add(cmd);
        else if (strcmp(cmd, "DELETE") == 0) handle_delete(cmd);
        else if (strcmp(cmd, "SHOW") == 0) handle_show(cmd);
        else if (strcmp(cmd, "REPORT") == 0) handle_report();
    }
    while (head) {
        Record *tmp = head;
        head = head->next;
        free_record(tmp);
    }
    return 0;
}