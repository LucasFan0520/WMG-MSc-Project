// F034.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Note {
    char *name;
    char *text;
    struct Note *next;
} Note;

Note *head = NULL;
Note *tail = NULL;

Note *find_note(const char *name) {
    Note *n = head;
    while (n) {
        if (strcmp(n->name, name) == 0) return n;
        n = n->next;
    }
    return NULL;
}

void add_note(const char *name, const char *text) {
    if (find_note(name)) return;
    Note *n = malloc(sizeof(Note));
    n->name = malloc(strlen(name)+1);
    strcpy(n->name, name);
    n->text = malloc(strlen(text)+1);
    strcpy(n->text, text);
    n->next = NULL;
    if (tail) tail->next = n;
    else head = n;
    tail = n;
}

void delete_note(const char *name) {
    Note *prev = NULL;
    Note *n = head;
    while (n) {
        if (strcmp(n->name, name) == 0) {
            if (prev) prev->next = n->next;
            else head = n->next;
            if (n == tail) tail = prev;
            free(n->name);
            free(n->text);
            free(n);
            return;
        }
        prev = n;
        n = n->next;
    }
}

void report_notes(void) {
    Note *n = head;
    while (n) {
        printf("%s %s\n", n->name, n->text);
        n = n->next;
    }
}

void load_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    Note *temp_head = NULL;
    Note *temp_tail = NULL;
    int valid = 1;
    char line[1024];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = 0;
        char *name = strtok(line, " ");
        if (!name) { valid = 0; break; }
        char *text = strtok(NULL, "");
        if (!text) text = "";
        Note *n = malloc(sizeof(Note));
        n->name = malloc(strlen(name)+1);
        strcpy(n->name, name);
        n->text = malloc(strlen(text)+1);
        strcpy(n->text, text);
        n->next = NULL;
        if (temp_tail) temp_tail->next = n;
        else temp_head = n;
        temp_tail = n;
    }
    fclose(f);
    if (valid && temp_head) {
        // free current store
        Note *n = head;
        while (n) {
            Note *next = n->next;
            free(n->name);
            free(n->text);
            free(n);
            n = next;
        }
        head = temp_head;
        tail = temp_tail;
    } else {
        // free temp
        Note *n = temp_head;
        while (n) {
            Note *next = n->next;
            free(n->name);
            free(n->text);
            free(n);
            n = next;
        }
    }
}

void free_all(void) {
    Note *n = head;
    while (n) {
        Note *next = n->next;
        free(n->name);
        free(n->text);
        free(n);
        n = next;
    }
}

int main(int argc, char **argv) {
    char *filepath = NULL;
    if (argc > 1) filepath = argv[1];
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        char *rest = strtok(NULL, "");
        if (!rest) rest = "";
        if (strcmp(cmd, "LOAD") == 0) {
            if (filepath) load_file(filepath);
        } else if (strcmp(cmd, "ADD") == 0) {
            char *name = strtok(rest, " ");
            if (!name) continue;
            char *text = strtok(NULL, "");
            if (!text) text = "";
            add_note(name, text);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = strtok(rest, " ");
            if (name) delete_note(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_notes();
        }
    }
    free_all();
    return 0;
}