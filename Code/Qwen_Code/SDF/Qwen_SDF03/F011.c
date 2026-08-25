// F011.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Draft {
    char *title, *body;
    struct Draft *next;
} Draft;

Draft *head = NULL, *discarded = NULL;

char *get_token(char **s) {
    while (**s == ' ') (*s)++;
    if (!**s) return NULL;
    char *start = *s;
    while (**s && **s != ' ') (*s)++;
    if (**s) { **s = '\0'; (*s)++; }
    return start;
}

Draft *find_draft(const char *title) {
    for (Draft *d = head; d; d = d->next)
        if (strcmp(d->title, title) == 0) return d;
    return NULL;
}

int main() {
    char line[65536];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *ptr = line;
        char *cmd = get_token(&ptr);
        if (!cmd) continue;
        if (strcmp(cmd, "CREATE") == 0) {
            char *title = get_token(&ptr);
            if (title && !find_draft(title)) {
                Draft *d = malloc(sizeof(Draft));
                if (d) { d->title = strdup(title); d->body = strdup(ptr); d->next = head; head = d; }
            }
        } else if (strcmp(cmd, "REVISE") == 0) {
            char *title = get_token(&ptr);
            if (title) {
                Draft *d = find_draft(title);
                if (d) { char *tmp = strdup(ptr); if (tmp) { free(d->body); d->body = tmp; } }
            }
        } else if (strcmp(cmd, "DISCARD") == 0) {
            char *title = get_token(&ptr);
            if (title) {
                Draft *prev = NULL, *curr = head;
                while (curr) {
                    if (strcmp(curr->title, title) == 0) {
                        if (prev) prev->next = curr->next; else head = curr->next;
                        if (discarded) { free(discarded->title); free(discarded->body); free(discarded); }
                        discarded = curr;
                        discarded->next = NULL;
                        break;
                    }
                    prev = curr; curr = curr->next;
                }
            }
        } else if (strcmp(cmd, "RESTORE") == 0) {
            if (discarded) {
                discarded->next = head;
                head = discarded;
                discarded = NULL;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *title = get_token(&ptr);
            if (title) {
                Draft *d = find_draft(title);
                if (d) printf("%s\n", d->body);
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (Draft *d = head; d; d = d->next) printf("%s %s\n", d->title, d->body);
        }
    }
    while (head) {
        Draft *next = head->next;
        free(head->title); free(head->body); free(head);
        head = next;
    }
    if (discarded) { free(discarded->title); free(discarded->body); free(discarded); }
    return 0;
}