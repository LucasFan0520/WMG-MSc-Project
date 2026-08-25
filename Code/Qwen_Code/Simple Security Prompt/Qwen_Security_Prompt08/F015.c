// F015.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct C {
    char *name;
    char *email;
    char *phone;
    char *note;
    struct C *next;
} C;

C *head = NULL;

int main() {
    char cmd[20];
    char name[100];
    char email[100];
    char phone[100];
    char note[1000];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "ADD")) {
            scanf("%s %s %s", name, email, phone);
            C *nc = malloc(sizeof(C));
            nc->name = strdup(name);
            nc->email = strdup(email);
            nc->phone = strdup(phone);
            nc->note = strdup("");
            nc->next = head;
            head = nc;
        } else if (!strcmp(cmd, "NOTE")) {
            scanf("%s", name);
            fgets(note, 1000, stdin);
            note[strcspn(note, "\n")] = 0;
            if (note[0] == ' ') {
                memmove(note, note + 1, strlen(note));
            }
            C *p = head;
            while (p && strcmp(p->name, name)) {
                p = p->next;
            }
            if (p) {
                free(p->note);
                p->note = strdup(note);
            }
        } else if (!strcmp(cmd, "MERGE")) {
            scanf("%s %s", name, email);
            C *k = head;
            while (k && strcmp(k->name, name)) {
                k = k->next;
            }
            C *r = head;
            while (r && strcmp(r->name, email)) {
                r = r->next;
            }
            if (k && r) {
                char *nn = malloc(strlen(k->note) + strlen(r->note) + 2);
                sprintf(nn, "%s %s", k->note, r->note);
                free(k->note);
                k->note = nn;
                C *p = head;
                C *prev = NULL;
                while (p) {
                    if (p == r) {
                        if (prev) {
                            prev->next = p->next;
                        } else {
                            head = p->next;
                        }
                        free(p->name);
                        free(p->email);
                        free(p->phone);
                        free(p->note);
                        free(p);
                        break;
                    }
                    prev = p;
                    p = p->next;
                }
            }
        } else if (!strcmp(cmd, "DELETE")) {
            scanf("%s", name);
            C *p = head;
            C *prev = NULL;
            while (p) {
                if (!strcmp(p->name, name)) {
                    if (prev) {
                        prev->next = p->next;
                    } else {
                        head = p->next;
                    }
                    free(p->name);
                    free(p->email);
                    free(p->phone);
                    free(p->note);
                    free(p);
                    break;
                }
                prev = p;
                p = p->next;
            }
        } else if (!strcmp(cmd, "FIND")) {
            scanf("%s", name);
            C *p = head;
            while (p && strcmp(p->name, name)) {
                p = p->next;
            }
            if (p) {
                printf("%s %s %s %s\n", p->name, p->email, p->phone, p->note);
            }
        } else if (!strcmp(cmd, "LIST")) {
            for (C *p = head; p; p = p->next) {
                printf("%s %s %s %s\n", p->name, p->email, p->phone, p->note);
            }
        }
    }
    return 0;
}