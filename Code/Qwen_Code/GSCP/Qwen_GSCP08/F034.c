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

Note *find_note(char *name) {
    for (Note *n = head; n; n = n->next) {
        if (strcmp(n->name, name) == 0) return n;
    }
    return NULL;
}

void add_note(char *name, char *text) {
    Note *n = find_note(name);
    if (!n) {
        n = malloc(sizeof(Note));
        n->name = strdup(name);
        n->next = head;
        head = n;
    } else {
        free(n->text);
    }
    n->text = strdup(text);
}

void delete_note(char *name) {
    Note *prev = NULL;
    Note *curr = head;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            free(curr->name);
            free(curr->text);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void report_notes() {
    for (Note *n = head; n; n = n->next) {
        printf("%s %s\n", n->name, n->text);
    }
}

void load_file(char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    
    long start_pos = ftell(f);
    char line[2000];
    int valid = 1;
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = 0;
        if (strlen(line) == 0) continue;
        char *space = strchr(line, ' ');
        if (!space) {
            valid = 0;
            break;
        }
    }
    
    if (!valid) {
        fclose(f);
        return;
    }
    
    fseek(f, start_pos, SEEK_SET);
    
    Note *new_head = NULL;
    Note *tail = NULL;
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = 0;
        if (strlen(line) == 0) continue;
        char *space = strchr(line, ' ');
        if (!space) continue;
        *space = 0;
        Note *n = malloc(sizeof(Note));
        n->name = strdup(line);
        n->text = strdup(space + 1);
        n->next = NULL;
        if (!new_head) new_head = n;
        else tail->next = n;
        tail = n;
    }
    fclose(f);
    
    while (head) {
        Note *next = head->next;
        free(head->name);
        free(head->text);
        free(head);
        head = next;
    }
    head = new_head;
}

int main(int argc, char *argv[]) {
    char cmd[20], a[100], text[2000];
    while (scanf("%19s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "LOAD") == 0) {
            if (argc > 1) load_file(argv[1]);
        } else if (strcmp(cmd, "ADD") == 0) {
            scanf("%99s", a);
            fgets(text, sizeof(text), stdin);
            while (text[0] == ' ') memmove(text, text + 1, strlen(text));
            text[strcspn(text, "\n")] = 0;
            add_note(a, text);
        } else if (strcmp(cmd, "DELETE") == 0) {
            scanf("%99s", a);
            delete_note(a);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_notes();
        }
    }
    return 0;
}