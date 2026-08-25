// F001.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Entry {
    char *id;
    char *text;
    struct Entry *next;
} Entry;

Entry *head = NULL;
Entry *tail = NULL;

Entry *find_entry(const char *id) {
    Entry *e = head;
    while (e) {
        if (strcmp(e->id, id) == 0) return e;
        e = e->next;
    }
    return NULL;
}

void add_entry(const char *id, const char *text) {
    if (find_entry(id)) return;
    Entry *e = malloc(sizeof(Entry));
    if (!e) return;
    e->id = strdup(id);
    e->text = strdup(text);
    e->next = NULL;
    if (tail) {
        tail->next = e;
        tail = e;
    } else {
        head = tail = e;
    }
}

void append_text(const char *id, const char *text) {
    Entry *e = find_entry(id);
    if (!e) return;
    size_t newlen = strlen(e->text) + strlen(text) + 1;
    char *newtext = malloc(newlen);
    if (!newtext) return;
    strcpy(newtext, e->text);
    strcat(newtext, text);
    free(e->text);
    e->text = newtext;
}

void replace_text(const char *id, const char *text) {
    Entry *e = find_entry(id);
    if (!e) return;
    free(e->text);
    e->text = strdup(text);
}

void delete_entry(const char *id) {
    Entry *prev = NULL;
    Entry *e = head;
    while (e) {
        if (strcmp(e->id, id) == 0) {
            if (prev) prev->next = e->next;
            else head = e->next;
            if (e == tail) tail = prev;
            free(e->id);
            free(e->text);
            free(e);
            return;
        }
        prev = e;
        e = e->next;
    }
}

void show_entry(const char *id) {
    Entry *e = find_entry(id);
    if (!e) {
        printf("NOT_FOUND\n");
        return;
    }
    for (char *p = e->text; *p; p++) {
        putchar(*p == ' ' ? '_' : *p);
    }
    putchar('\n');
}

void list_all(void) {
    Entry *e = head;
    while (e) {
        printf("%s\n", e->id);
        e = e->next;
    }
}

void free_all(void) {
    while (head) {
        Entry *next = head->next;
        free(head->id);
        free(head->text);
        free(head);
        head = next;
    }
    tail = NULL;
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;

    while ((nread = getline(&line, &len, stdin)) != -1) {
        if (line[nread-1] == '\n') line[nread-1] = '\0';
        char cmd[32], id[256], text[1024];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;

        if (strcmp(cmd, "NEW") == 0) {
            if (sscanf(line, "%*s %255s %1023[^\n]", id, text) == 2) {
                add_entry(id, text);
            }
        } else if (strcmp(cmd, "APPEND") == 0) {
            if (sscanf(line, "%*s %255s %1023[^\n]", id, text) == 2) {
                append_text(id, text);
            }
        } else if (strcmp(cmd, "REPLACE") == 0) {
            if (sscanf(line, "%*s %255s %1023[^\n]", id, text) == 2) {
                replace_text(id, text);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", id) == 1) {
                delete_entry(id);
            }
        } else if (strcmp(cmd, "SHOW") == 0) {
            if (sscanf(line, "%*s %255s", id) == 1) {
                show_entry(id);
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            list_all();
        }
    }

    free(line);
    free_all();
    return 0;
}