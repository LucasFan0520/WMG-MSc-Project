// F034.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct note {
    char *name;
    char *text;
    struct note *next;
};

struct note *notes = NULL;
struct note *backup = NULL;

void clear_notes(struct note **list) {
    while (*list) {
        struct note *n = *list;
        *list = n->next;
        free(n->name);
        free(n->text);
        free(n);
    }
}

void add_note(struct note **list, char *name, char *text) {
    struct note *n = malloc(sizeof(struct note));
    n->name = malloc(strlen(name) + 1);
    strcpy(n->name, name);
    n->text = malloc(strlen(text) + 1);
    strcpy(n->text, text);
    n->next = *list;
    *list = n;
}

void delete_note(struct note **list, char *name) {
    struct note *n = *list, *prev = NULL;
    while (n) {
        if (strcmp(n->name, name) == 0) {
            if (prev) prev->next = n->next;
            else *list = n->next;
            free(n->name);
            free(n->text);
            free(n);
            return;
        }
        prev = n;
        n = n->next;
    }
}

void report_notes(struct note *list) {
    struct note *n = list;
    while (n) {
        printf("%s %s\n", n->name, n->text);
        n = n->next;
    }
}

int load_file(char *path, struct note **target) {
    FILE *f = fopen(path, "r");
    if (!f) return 0;
    struct note *temp = NULL;
    char line[4096];
    int valid = 1;
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = 0;
        char name[256], text[4096];
        if (sscanf(line, "%255s %4095[^\n]", name, text) != 2) {
            valid = 0;
            break;
        }
        add_note(&temp, name, text);
    }
    fclose(f);
    if (valid) {
        // replace target with temp
        clear_notes(target);
        *target = temp;
        return 1;
    } else {
        clear_notes(&temp);
        return 0;
    }
}

int main(int argc, char **argv) {
    if (argc < 2) return 0;
    char *filepath = argv[1];
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char arg1[256], arg2[4096];
        int n = sscanf(line, "%31s %255s %4095[^\n]", cmd, arg1, arg2);
        if (strcmp(cmd, "LOAD") == 0) {
            // backup current
            clear_notes(&backup);
            backup = notes;
            notes = NULL;
            if (!load_file(filepath, &notes)) {
                // restore backup
                clear_notes(&notes);
                notes = backup;
                backup = NULL;
            } else {
                // success, discard backup
                clear_notes(&backup);
                backup = NULL;
            }
        } else if (strcmp(cmd, "ADD") == 0 && n >= 2) {
            char *rest = line + strlen(cmd) + 1 + strlen(arg1) + 1;
            add_note(&notes, arg1, rest);
        } else if (strcmp(cmd, "DELETE") == 0 && n >= 2) {
            delete_note(&notes, arg1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_notes(notes);
        }
    }
    clear_notes(&notes);
    clear_notes(&backup);
    return 0;
}