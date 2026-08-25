// F020.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096
#define MAX_TITLE 256
#define MAX_BODY 4096

typedef struct Record {
    char title[MAX_TITLE];
    char body[MAX_BODY];
    struct Record *next;
} Record;

Record *head = NULL;

Record *find_record(const char *title) {
    Record *r = head;
    while (r) {
        if (strcmp(r->title, title) == 0) return r;
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

void load_file(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) return;
    char line[MAX_LINE];
    Record *current = NULL;
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "--") == 0) {
            current = NULL;
            continue;
        }
        if (!current) {
            // title line
            Record *r = malloc(sizeof(Record));
            if (!r) continue;
            strcpy(r->title, line);
            r->body[0] = '\0';
            r->next = head;
            head = r;
            current = r;
        } else {
            // body line
            if (strlen(current->body) + strlen(line) + 1 < MAX_BODY) {
                if (current->body[0]) strcat(current->body, "\n");
                strcat(current->body, line);
            }
        }
    }
    fclose(f);
}

void handle_find(char *line) {
    char title[MAX_TITLE];
    if (sscanf(line, "FIND %255s", title) != 1) return;
    Record *r = find_record(title);
    if (!r) return;
    printf("%s\n%s\n", r->title, r->body);
}

void handle_delete(char *line) {
    char title[MAX_TITLE];
    if (sscanf(line, "DELETE %255s", title) != 1) return;
    Record *r = head, *prev = NULL;
    while (r) {
        if (strcmp(r->title, title) == 0) {
            if (prev) prev->next = r->next;
            else head = r->next;
            free(r);
            return;
        }
        prev = r;
        r = r->next;
    }
}

void handle_add() {
    // Reads following body lines until "--"
    char title[MAX_TITLE], body[MAX_BODY] = "";
    if (!fgets(title, sizeof(title), stdin)) return;
    title[strcspn(title, "\n")] = '\0';
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "--") == 0) break;
        if (strlen(body) + strlen(line) + 1 < MAX_BODY) {
            if (body[0]) strcat(body, "\n");
            strcat(body, line);
        }
    }
    if (find_record(title)) return;
    Record *r = malloc(sizeof(Record));
    if (!r) return;
    strcpy(r->title, title);
    strcpy(r->body, body);
    r->next = head;
    head = r;
}

void handle_print() {
    Record *r = head;
    while (r) {
        printf("%s\n%s\n--\n", r->title, r->body);
        r = r->next;
    }
}

int main(int argc, char **argv) {
    if (argc > 1) load_file(argv[1]);
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        if (strncmp(line, "FIND ", 5) == 0) handle_find(line);
        else if (strncmp(line, "DELETE ", 7) == 0) handle_delete(line);
        else if (strcmp(line, "ADD") == 0) handle_add();
        else if (strcmp(line, "PRINT") == 0) handle_print();
    }
    free_all();
    return 0;
}