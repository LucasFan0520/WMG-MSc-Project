// F011.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line() {
    size_t cap = 256, len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    while (1) {
        if (!fgets(buf + len, cap - len, stdin)) {
            if (len == 0) { free(buf); return NULL; }
            break;
        }
        len = strlen(buf);
        if (len > 0 && buf[len-1] == '\n') { buf[len-1] = '\0'; break; }
        if (len == cap - 1) {
            cap *= 2;
            char *tmp = realloc(buf, cap);
            if (!tmp) { free(buf); return NULL; }
            buf = tmp;
        }
    }
    return buf;
}

typedef struct Draft {
    char *title;
    char *body;
    struct Draft *next;
} Draft;

Draft *drafts = NULL;
Draft *discard_stack = NULL;

Draft *find_draft(const char *title) {
    for (Draft *d = drafts; d; d = d->next) {
        if (strcmp(d->title, title) == 0) return d;
    }
    return NULL;
}

void create_draft(const char *title, const char *body) {
    Draft *d = malloc(sizeof(Draft));
    d->title = strdup(title);
    d->body = strdup(body);
    d->next = drafts;
    drafts = d;
}

void revise_draft(const char *title, const char *body) {
    Draft *d = find_draft(title);
    if (d) {
        free(d->body);
        d->body = strdup(body);
    }
}

void discard_draft(const char *title) {
    Draft *prev = NULL, *curr = drafts;
    while (curr) {
        if (strcmp(curr->title, title) == 0) {
            if (prev) prev->next = curr->next;
            else drafts = curr->next;
            curr->next = discard_stack;
            discard_stack = curr;
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void restore_draft() {
    if (discard_stack) {
        Draft *d = discard_stack;
        discard_stack = discard_stack->next;
        d->next = drafts;
        drafts = d;
    }
}

void find_print_draft(const char *title) {
    Draft *d = find_draft(title);
    if (d) {
        printf("%s\n", d->body);
    }
}

void print_all_drafts() {
    for (Draft *d = drafts; d; d = d->next) {
        printf("%s: %s\n", d->title, d->body);
    }
}

int main() {
    char *line;
    while ((line = read_line())) {
        if (strcmp(line, "END") == 0) break;
        char cmd[16] = {0}, title[256] = {0};
        sscanf(line, "%15s %255s", cmd, title);
        char *body = strchr(line, ' ');
        if (body) {
            body++;
            while (*body == ' ') body++;
            body = strchr(body, ' ');
            if (body) {
                body++;
                while (*body == ' ') body++;
            } else {
                body = NULL;
            }
        }
        
        if (strcmp(cmd, "CREATE") == 0 && body) {
            create_draft(title, body);
        } else if (strcmp(cmd, "REVISE") == 0 && body) {
            revise_draft(title, body);
        } else if (strcmp(cmd, "DISCARD") == 0) {
            discard_draft(title);
        } else if (strcmp(cmd, "RESTORE") == 0) {
            restore_draft();
        } else if (strcmp(cmd, "FIND") == 0) {
            find_print_draft(title);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_all_drafts();
        }
        free(line);
    }
    
    Draft *d = drafts;
    while (d) {
        Draft *next = d->next;
        free(d->title);
        free(d->body);
        free(d);
        d = next;
    }
    d = discard_stack;
    while (d) {
        Draft *next = d->next;
        free(d->title);
        free(d->body);
        free(d);
        d = next;
    }
    return 0;
}