// F011.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Draft {
    char *title;
    char *body;
    struct Draft *next;
} Draft;

Draft *head = NULL;
Draft *trash = NULL;

void create_draft(const char *title, const char *body) {
    Draft *d = malloc(sizeof(Draft));
    if (!d) return;
    d->title = strdup(title);
    d->body = strdup(body);
    d->next = head;
    head = d;
}

void revise_draft(const char *title, const char *body) {
    for (Draft *curr = head; curr; curr = curr->next) {
        if (strcmp(curr->title, title) == 0) {
            free(curr->body);
            curr->body = strdup(body);
            return;
        }
    }
}

void discard_draft(const char *title) {
    Draft *curr = head;
    Draft *prev = NULL;
    while (curr) {
        if (strcmp(curr->title, title) == 0) {
            Draft *next = curr->next;
            if (prev) prev->next = next;
            else head = next;
            
            curr->next = trash;
            trash = curr;
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void restore_draft() {
    if (!trash) return;
    Draft *d = trash;
    trash = trash->next;
    d->next = head;
    head = d;
}

void find_draft(const char *title) {
    for (Draft *curr = head; curr; curr = curr->next) {
        if (strcmp(curr->title, title) == 0) {
            printf("%s %s\n", curr->title, curr->body);
            return;
        }
    }
}

void print_drafts() {
    for (Draft *curr = head; curr; curr = curr->next) {
        printf("%s %s\n", curr->title, curr->body);
    }
}

void free_list(Draft *list) {
    while (list) {
        Draft *next = list->next;
        free(list->title);
        free(list->body);
        free(list);
        list = next;
    }
}

void free_all() {
    free_list(head);
    free_list(trash);
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[16];
        char title[1024], body[4096];
        
        int n = sscanf(line, "%15s %1023s %[^\n]", cmd, title, body);
        if (strcmp(cmd, "CREATE") == 0 && n == 3) {
            create_draft(title, body);
        } else if (strcmp(cmd, "REVISE") == 0 && n == 3) {
            revise_draft(title, body);
        } else if (strcmp(cmd, "DISCARD") == 0 && n >= 2) {
            discard_draft(title);
        } else if (strcmp(cmd, "RESTORE") == 0) {
            restore_draft();
        } else if (strcmp(cmd, "FIND") == 0 && n >= 2) {
            find_draft(title);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_drafts();
        }
    }
    free_all();
    return 0;
}