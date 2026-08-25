// F015.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Contact {
    char *name, *email, *phone, *note;
    struct Contact *next;
} Contact;

Contact *head = NULL;

void read_rest(char *buf) {
    if (!fgets(buf, 1024, stdin)) buf[0] = '\0';
    if (buf[0] == ' ') memmove(buf, buf + 1, strlen(buf));
    size_t l = strlen(buf);
    if (l > 0 && buf[l-1] == '\n') buf[l-1] = '\0';
}

int main() {
    char cmd[20], n1[256], n2[256], text[1024];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "ADD") == 0) {
            scanf("%s %s %s", n1, n2, text);
            Contact *c = malloc(sizeof(Contact));
            c->name = strdup(n1); c->email = strdup(n2); c->phone = strdup(text); c->note = strdup(""); c->next = head; head = c;
        } else if (strcmp(cmd, "NOTE") == 0) {
            scanf("%s", n1); read_rest(text);
            for (Contact *c = head; c; c = c->next)
                if (strcmp(c->name, n1) == 0) { free(c->note); c->note = strdup(text); break; }
        } else if (strcmp(cmd, "MERGE") == 0) {
            scanf("%s %s", n1, n2);
            Contact *keep = NULL, *rem = NULL;
            for (Contact *c = head; c; c = c->next) {
                if (strcmp(c->name, n1) == 0) keep = c;
                if (strcmp(c->name, n2) == 0) rem = c;
            }
            if (keep && rem) {
                char *new_note = malloc(strlen(keep->note) + strlen(rem->note) + 2);
                sprintf(new_note, "%s %s", keep->note, rem->note);
                free(keep->note); keep->note = new_note;
                
                Contact *prev = NULL, *curr = head;
                while (curr) {
                    if (curr == rem) {
                        if (prev) prev->next = curr->next; else head = curr->next;
                        free(curr->name); free(curr->email); free(curr->phone); free(curr->note); free(curr); break;
                    }
                    prev = curr; curr = curr->next;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            scanf("%s", n1);
            Contact *prev = NULL, *curr = head;
            while (curr) {
                if (strcmp(curr->name, n1) == 0) {
                    if (prev) prev->next = curr->next; else head = curr->next;
                    free(curr->name); free(curr->email); free(curr->phone); free(curr->note); free(curr); break;
                }
                prev = curr; curr = curr->next;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            scanf("%s", n1);
            for (Contact *c = head; c; c = c->next)
                if (strcmp(c->name, n1) == 0) { printf("%s %s %s %s\n", c->name, c->email, c->phone, c->note); break; }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (Contact *c = head; c; c = c->next) printf("%s %s %s %s\n", c->name, c->email, c->phone, c->note);
        }
    }
    return 0;
}