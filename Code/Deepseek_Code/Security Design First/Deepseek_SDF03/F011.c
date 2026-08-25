// F011.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096
#define MAX_TITLE 64
#define MAX_BODY 4096

typedef struct Draft {
    char title[MAX_TITLE];
    char body[MAX_BODY];
    struct Draft *next;
} Draft;

Draft *head = NULL;
Draft *discarded = NULL;  // most recently discarded

Draft *find_draft(const char *title) {
    Draft *d = head;
    while (d) {
        if (strcmp(d->title, title) == 0) return d;
        d = d->next;
    }
    return NULL;
}

void free_draft(Draft *d) {
    if (d) free(d);
}

void free_all() {
    Draft *d = head;
    while (d) {
        Draft *tmp = d;
        d = d->next;
        free(tmp);
    }
    head = NULL;
    if (discarded) { free(discarded); discarded = NULL; }
}

void handle_create(char *line) {
    char title[MAX_TITLE], body[MAX_BODY];
    int n = sscanf(line, "CREATE %63s %4095[^\n]", title, body);
    if (n < 2) return;
    if (find_draft(title)) return;
    Draft *d = malloc(sizeof(Draft));
    if (!d) return;
    strcpy(d->title, title);
    strcpy(d->body, body);
    d->next = head;
    head = d;
}

void handle_revise(char *line) {
    char title[MAX_TITLE], body[MAX_BODY];
    int n = sscanf(line, "REVISE %63s %4095[^\n]", title, body);
    if (n < 2) return;
    Draft *d = find_draft(title);
    if (!d) return;
    strcpy(d->body, body);
}

void handle_discard(char *line) {
    char title[MAX_TITLE];
    if (sscanf(line, "DISCARD %63s", title) != 1) return;
    Draft *d = head, *prev = NULL;
    while (d) {
        if (strcmp(d->title, title) == 0) {
            if (prev) prev->next = d->next;
            else head = d->next;
            if (discarded) free(discarded);
            discarded = d;
            return;
        }
        prev = d;
        d = d->next;
    }
}

void handle_restore() {
    if (!discarded) return;
    // Reinsert at head
    discarded->next = head;
    head = discarded;
    discarded = NULL;
}

void handle_find(char *line) {
    char title[MAX_TITLE];
    if (sscanf(line, "FIND %63s", title) != 1) return;
    Draft *d = find_draft(title);
    if (!d) return;
    printf("%s\n", d->body);
}

void handle_print() {
    Draft *d = head;
    while (d) {
        printf("%s %s\n", d->title, d->body);
        d = d->next;
    }
}

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        if (strncmp(line, "CREATE ", 7) == 0) handle_create(line);
        else if (strncmp(line, "REVISE ", 7) == 0) handle_revise(line);
        else if (strncmp(line, "DISCARD ", 8) == 0) handle_discard(line);
        else if (strcmp(line, "RESTORE") == 0) handle_restore();
        else if (strncmp(line, "FIND ", 5) == 0) handle_find(line);
        else if (strcmp(line, "PRINT") == 0) handle_print();
    }
    free_all();
    return 0;
}