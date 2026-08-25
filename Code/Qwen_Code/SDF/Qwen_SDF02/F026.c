// F026.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 4096

typedef struct Note {
    char *category;
    char *text;
    struct Note *next;
} Note;

Note *notes = NULL;

void print_text(const char *text) {
    for (const char *p = text; *p; p++) putchar(*p == ' ' ? '_' : *p);
    putchar('\n');
}

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        
        if (strcmp(cmd, "ADD") == 0) {
            char cat[256];
            int n = 0;
            if (sscanf(rest, "%255s%n", cat, &n) != 1) continue;
            char *text = rest + n;
            while (*text == ' ') text++;
            Note *note = malloc(sizeof(Note));
            if (!note) continue;
            note->category = strdup(cat);
            note->text = strdup(text);
            if (!note->category || !note->text) {
                free(note->category); free(note->text); free(note); continue;
            }
            note->next = notes;
            notes = note;
        } else if (strcmp(cmd, "DELETE") == 0) {
            char cat[256];
            if (sscanf(rest, "%255s", cat) != 1) continue;
            Note *prev = NULL;
            for (Note *n = notes; n; ) {
                if (strcmp(n->category, cat) == 0) {
                    Note *to_del = n;
                    n = n->next;
                    if (prev) prev->next = n;
                    else notes = n;
                    free(to_del->category);
                    free(to_del->text);
                    free(to_del);
                } else {
                    prev = n;
                    n = n->next;
                }
            }
        } else if (strcmp(cmd, "KEEP") == 0) {
            char cat[256];
            if (sscanf(rest, "%255s", cat) != 1) continue;
            Note *prev = NULL;
            for (Note *n = notes; n; ) {
                if (strcmp(n->category, cat) != 0) {
                    Note *to_del = n;
                    n = n->next;
                    if (prev) prev->next = n;
                    else notes = n;
                    free(to_del->category);
                    free(to_del->text);
                    free(to_del);
                } else {
                    prev = n;
                    n = n->next;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char word[256];
            if (sscanf(rest, "%255s", word) != 1) continue;
            for (Note *n = notes; n; n = n->next) {
                if (strstr(n->text, word)) {
                    print_text(n->text);
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (Note *n = notes; n; n = n->next) {
                printf("%s ", n->category);
                print_text(n->text);
            }
        }
    }
    while (notes) {
        Note *n = notes;
        notes = notes->next;
        free(n->category);
        free(n->text);
        free(n);
    }
    return 0;
}