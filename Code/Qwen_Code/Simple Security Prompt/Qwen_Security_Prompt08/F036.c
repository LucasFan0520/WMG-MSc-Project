// F036.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Rec {
    char *name;
    char *data;
    struct Rec *next;
} Rec;

Rec *head = NULL;

int main() {
    char cmd[20];
    char name[100];
    char raw[2000];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "ADD")) {
            scanf("%s %s", name, raw);
            char *colon = strchr(raw, ':');
            if (colon) {
                *colon = 0;
                int len = atoi(raw);
                char *data = colon + 1;
                if ((int)strlen(data) == len) {
                    Rec *r = malloc(sizeof(Rec));
                    r->name = strdup(name);
                    r->data = strdup(data);
                    r->next = head;
                    head = r;
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
                    free(p->data);
                    free(p);
                    break;
                }
                prev = p;
                p = p->next;
            }
        } else if (!strcmp(cmd, "SHOW")) {
            scanf("%s", name);
            for (Rec *r = head; r; r = r->next) {
                if (!strcmp(r->name, name)) {
                    for (char *c = r->data; *c; c++) {
                        if (*c == ' ') printf("_");
                        else printf("%c", *c);
                    }
                    printf("\n");
                    break;
                }
            }
        } else if (!strcmp(cmd, "REPORT")) {
            for (Rec *r = head; r; r = r->next) {
                printf("%s\n", r->name);
            }
        }
    }
    return 0;
}