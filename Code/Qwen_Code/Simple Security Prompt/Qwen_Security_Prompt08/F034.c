// F034.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Note {
    char *name;
    char *note;
    struct Note *next;
} Note;

Note *head = NULL;

void load_file(char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    Note *temp = NULL;
    char line[2000];
    int valid = 1;
    while (fgets(line, 2000, f)) {
        line[strcspn(line, "\n")] = 0;
        char *space = strchr(line, ' ');
        if (!space) {
            valid = 0;
            break;
        }
        *space = 0;
        Note *n = malloc(sizeof(Note));
        n->name = strdup(line);
        n->note = strdup(space + 1);
        n->next = temp;
        temp = n;
    }
    fclose(f);
    if (valid) {
        while (head) {
            Note *tmp = head;
            head = head->next;
            free(tmp->name);
            free(tmp->note);
            free(tmp);
        }
        head = temp;
    } else {
        while (temp) {
            Note *tmp = temp;
            temp = temp->next;
            free(tmp->name);
            free(tmp->note);
            free(tmp);
        }
    }
}

int main(int argc, char **argv) {
    char cmd[20];
    char name[100];
    char note[1000];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "LOAD")) {
            if (argc > 1) load_file(argv[1]);
        } else if (!strcmp(cmd, "ADD")) {
            scanf("%s", name);
            fgets(note, 1000, stdin);
            note[strcspn(note, "\n")] = 0;
            if (note[0] == ' ') memmove(note, note + 1, strlen(note));
            Note *n = malloc(sizeof(Note));
            n->name = strdup(name);
            n->note = strdup(note);
            n->next = head;
            head = n;
        } else if (!strcmp(cmd, "DELETE")) {
            scanf("%s", name);
            Note *p = head;
            Note *prev = NULL;
            while (p) {
                if (!strcmp(p->name, name)) {
                    if (prev) prev->next = p->next;
                    else head = p->next;
                    free(p->name);
                    free(p->note);
                    free(p);
                    break;
                }
                prev = p;
                p = p->next;
            }
        } else if (!strcmp(cmd, "REPORT")) {
            for (Note *n = head; n; n = n->next) {
                printf("%s %s\n", n->name, n->note);
            }
        }
    }
    return 0;
}