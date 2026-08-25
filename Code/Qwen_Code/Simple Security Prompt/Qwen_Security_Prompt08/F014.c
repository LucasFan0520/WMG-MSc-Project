// F014.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Lb {
    char *label;
    struct Lb *next;
} Lb;

typedef struct It {
    char *name;
    char *note;
    Lb *labels;
    struct It *next;
} It;

It *head = NULL;

int main() {
    char cmd[20];
    char name[100];
    char note[1000];
    char label[100];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "ITEM")) {
            scanf("%s", name);
            fgets(note, 1000, stdin);
            note[strcspn(note, "\n")] = 0;
            if (note[0] == ' ') {
                memmove(note, note + 1, strlen(note));
            }
            It *ni = malloc(sizeof(It));
            ni->name = strdup(name);
            ni->note = strdup(note);
            ni->labels = NULL;
            ni->next = head;
            head = ni;
        } else if (!strcmp(cmd, "LABEL")) {
            scanf("%s %s", name, label);
            It *p = head;
            while (p && strcmp(p->name, name)) {
                p = p->next;
            }
            if (p) {
                Lb *nl = malloc(sizeof(Lb));
                nl->label = strdup(label);
                nl->next = p->labels;
                p->labels = nl;
            }
        } else if (!strcmp(cmd, "UNLABEL")) {
            scanf("%s %s", name, label);
            It *p = head;
            while (p && strcmp(p->name, name)) {
                p = p->next;
            }
            if (p) {
                Lb *l = p->labels;
                Lb *prev = NULL;
                while (l) {
                    if (!strcmp(l->label, label)) {
                        if (prev) {
                            prev->next = l->next;
                        } else {
                            p->labels = l->next;
                        }
                        free(l->label);
                        free(l);
                        break;
                    }
                    prev = l;
                    l = l->next;
                }
            }
        } else if (!strcmp(cmd, "DELETE")) {
            scanf("%s", name);
            It *p = head;
            It *prev = NULL;
            while (p) {
                if (!strcmp(p->name, name)) {
                    if (prev) {
                        prev->next = p->next;
                    } else {
                        head = p->next;
                    }
                    Lb *l = p->labels;
                    while (l) {
                        Lb *tmp = l;
                        l = l->next;
                        free(tmp->label);
                        free(tmp);
                    }
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
            It *p = head;
            while (p && strcmp(p->name, name)) {
                p = p->next;
            }
            if (p) {
                printf("%s %s", p->name, p->note);
                for (Lb *l = p->labels; l; l = l->next) {
                    printf(" %s", l->label);
                }
                printf("\n");
            }
        } else if (!strcmp(cmd, "REPORT")) {
            for (It *p = head; p; p = p->next) {
                printf("%s %s", p->name, p->note);
                for (Lb *l = p->labels; l; l = l->next) {
                    printf(" %s", l->label);
                }
                printf("\n");
            }
        }
    }
    return 0;
}