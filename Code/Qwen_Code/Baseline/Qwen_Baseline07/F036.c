// F036.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Rec {
    char *name, *data;
    struct Rec *next;
} Rec;

Rec *head = NULL;

int main() {
    char cmd[20], name[256], raw[2048];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "ADD") == 0) {
            scanf("%s %s", name, raw);
            char *colon = strchr(raw, ':');
            if (!colon) continue;
            *colon = '\0';
            int expected_len = atoi(raw);
            char *data = colon + 1;
            if ((int)strlen(data) != expected_len) continue;
            Rec *r = malloc(sizeof(Rec));
            r->name = strdup(name); r->data = strdup(data); r->next = head; head = r;
        } else if (strcmp(cmd, "DELETE") == 0) {
            scanf("%s", name);
            Rec *prev = NULL, *curr = head;
            while (curr) {
                if (strcmp(curr->name, name) == 0) {
                    if (prev) prev->next = curr->next; else head = curr->next;
                    free(curr->name); free(curr->data); free(curr); break;
                }
                prev = curr; curr = curr->next;
            }
        } else if (strcmp(cmd, "SHOW") == 0) {
            scanf("%s", name);
            for (Rec *r = head; r; r = r->next) {
                if (strcmp(r->name, name) == 0) {
                    for (char *c = r->data; *c; c++) printf("%c", *c == ' ' ? '_' : *c);
                    printf("\n");
                    break;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (Rec *r = head; r; r = r->next) printf("%s\n", r->name);
        }
    }
    return 0;
}