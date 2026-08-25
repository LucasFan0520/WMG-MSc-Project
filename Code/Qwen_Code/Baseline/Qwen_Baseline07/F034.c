// F034.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Note {
    char *name, *note;
    struct Note *next;
} Note;

Note *head = NULL;

void read_rest(char *buf, int size) {
    if (!fgets(buf, size, stdin)) { buf[0] = '\0'; return; }
    if (buf[0] == ' ') memmove(buf, buf + 1, strlen(buf));
    size_t len = strlen(buf);
    if (len > 0 && buf[len-1] == '\n') buf[len-1] = '\0';
}

void load_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    Note *new_head = NULL;
    char line[2048];
    int valid = 1;
    while (fgets(line, sizeof(line), f)) {
        size_t l = strlen(line);
        if (l > 0 && line[l-1] == '\n') line[l-1] = '\0';
        char *space = strchr(line, ' ');
        if (!space) { valid = 0; break; }
        *space = '\0';
        Note *n = malloc(sizeof(Note));
        n->name = strdup(line); n->note = strdup(space + 1); n->next = new_head; new_head = n;
    }
    fclose(f);
    if (valid) {
        while (head) { Note *n = head->next; free(head->name); free(head->note); free(head); head = n; }
        head = new_head;
    } else {
        while (new_head) { Note *n = new_head->next; free(new_head->name); free(new_head->note); free(new_head); new_head = n; }
    }
}

int main(int argc, char *argv[]) {
    char cmd[20], name[256], note[1024];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "LOAD") == 0) {
            if (argc > 1) load_file(argv[1]);
        } else if (strcmp(cmd, "ADD") == 0) {
            scanf("%s", name); read_rest(note, sizeof(note));
            Note *n = malloc(sizeof(Note));
            n->name = strdup(name); n->note = strdup(note); n->next = head; head = n;
        } else if (strcmp(cmd, "DELETE") == 0) {
            scanf("%s", name);
            Note *prev = NULL, *curr = head;
            while (curr) {
                if (strcmp(curr->name, name) == 0) {
                    if (prev) prev->next = curr->next; else head = curr->next;
                    free(curr->name); free(curr->note); free(curr); break;
                }
                prev = curr; curr = curr->next;
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (Note *n = head; n; n = n->next) printf("%s %s\n", n->name, n->note);
        }
    }
    return 0;
}