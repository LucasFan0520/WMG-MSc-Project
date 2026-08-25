// F034.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096
#define MAX_NAME 64
#define MAX_NOTE 4096

typedef struct Note {
    char name[MAX_NAME];
    char note[MAX_NOTE];
    struct Note *next;
} Note;

Note *head = NULL;

Note *find_note(const char *name) {
    Note *n = head;
    while (n) {
        if (strcmp(n->name, name) == 0) return n;
        n = n->next;
    }
    return NULL;
}

void free_notes(Note *list) {
    while (list) {
        Note *tmp = list;
        list = list->next;
        free(tmp);
    }
}

void handle_load(char *line, const char *filename) {
    // save current store
    Note *old_head = head;
    head = NULL;
    // read file
    FILE *f = fopen(filename, "r");
    if (!f) {
        // restore old
        head = old_head;
        return;
    }
    char buf[MAX_LINE];
    int valid = 1;
    Note *new_list = NULL;
    while (fgets(buf, sizeof(buf), f)) {
        buf[strcspn(buf, "\n")] = '\0';
        char name[MAX_NAME], note[MAX_NOTE];
        if (sscanf(buf, "%63s %4095[^\n]", name, note) != 2) {
            valid = 0;
            break;
        }
        Note *n = malloc(sizeof(Note));
        if (!n) { valid = 0; break; }
        strcpy(n->name, name);
        strcpy(n->note, note);
        n->next = new_list;
        new_list = n;
    }
    fclose(f);
    if (valid) {
        // replace old with new
        free_notes(old_head);
        head = new_list;
    } else {
        // discard new, restore old
        free_notes(new_list);
        head = old_head;
    }
}

void handle_add(char *line) {
    char name[MAX_NAME], note[MAX_NOTE];
    int n = sscanf(line, "ADD %63s %4095[^\n]", name, note);
    if (n < 2) return;
    if (find_note(name)) return;
    Note *new = malloc(sizeof(Note));
    if (!new) return;
    strcpy(new->name, name);
    strcpy(new->note, note);
    new->next = head;
    head = new;
}

void handle_delete(char *line) {
    char name[MAX_NAME];
    if (sscanf(line, "DELETE %63s", name) != 1) return;
    Note *n = head, *prev = NULL;
    while (n) {
        if (strcmp(n->name, name) == 0) {
            if (prev) prev->next = n->next;
            else head = n->next;
            free(n);
            return;
        }
        prev = n;
        n = n->next;
    }
}

void handle_report() {
    Note *n = head;
    while (n) {
        printf("%s %s\n", n->name, n->note);
        n = n->next;
    }
}

int main(int argc, char **argv) {
    if (argc < 2) return 1;
    char *filename = argv[1];
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        if (strcmp(line, "LOAD") == 0) {
            handle_load(line, filename);
        } else if (strncmp(line, "ADD ", 4) == 0) {
            handle_add(line);
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            handle_delete(line);
        } else if (strcmp(line, "REPORT") == 0) {
            handle_report();
        }
    }
    free_notes(head);
    return 0;
}