// F031.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Rec {
    char *name;
    char *note;
    struct Rec *next;
} Rec;

Rec *head = NULL;

int main() {
    char cmd[20];
    char name[100];
    char note[1000];
    int n;
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "BATCH")) {
            scanf("%d", &n);
            Rec *temp[1000];
            int valid = 1;
            for (int i = 0; i < n; i++) {
                if (scanf("%s", name) != 1) {
                    valid = 0;
                    break;
                }
                fgets(note, 1000, stdin);
                note[strcspn(note, "\n")] = 0;
                if (note[0] == ' ') memmove(note, note + 1, strlen(note));
                if (strlen(name) == 0 || strlen(note) == 0) {
                    valid = 0;
                }
                temp[i] = malloc(sizeof(Rec));
                temp[i]->name = strdup(name);
                temp[i]->note = strdup(note);
            }
            if (valid) {
                for (int i = 0; i < n; i++) {
                    temp[i]->next = head;
                    head = temp[i];
                }
            } else {
                for (int i = 0; i < n; i++) {
                    if (temp[i]) {
                        free(temp[i]->name);
                        free(temp[i]->note);
                        free(temp[i]);
                    }
                }
            }
        } else if (!strcmp(cmd, "DELETE")) {
            scanf("%s", name);
            Rec *p = head;
            Rec *prev = NULL;
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
        } else if (!strcmp(cmd, "FIND")) {
            scanf("%s", name);
            for (Rec *r = head; r; r = r->next) {
                if (!strcmp(r->name, name)) {
                    printf("%s\n", r->note);
                    break;
                }
            }
        } else if (!strcmp(cmd, "REPORT")) {
            for (Rec *r = head; r; r = r->next) {
                printf("%s %s\n", r->name, r->note);
            }
        }
    }
    return 0;
}