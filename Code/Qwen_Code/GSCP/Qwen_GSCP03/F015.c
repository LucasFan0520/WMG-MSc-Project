// F015.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Contact {
    char *name, *email, *phone, *note;
    struct Contact *next;
} Contact;

Contact *head = NULL;

void free_list() {
    while (head) {
        Contact *t = head; head = head->next;
        free(t->name); free(t->email); free(t->phone); free(t->note); free(t);
    }
}

Contact *find_contact(const char *name) {
    for (Contact *c = head; c; c = c->next)
        if (strcmp(c->name, name) == 0) return c;
    return NULL;
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        char cmd[16], n1[256], n2[256], v1[256], v2[256], note[4096] = "";
        if (sscanf(line, "%15s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;

        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%*s %255s %255s %255s", n1, v1, v2) != 3) continue;
            if (find_contact(n1)) continue;
            Contact *c = malloc(sizeof(Contact));
            c->name = strdup(n1); c->email = strdup(v1); c->phone = strdup(v2);
            c->note = strdup(""); c->next = head; head = c;
        } else if (strcmp(cmd, "NOTE") == 0) {
            if (sscanf(line, "%*s %255s %4095[^\n]", n1, note) < 1) continue;
            Contact *c = find_contact(n1);
            if (c) { free(c->note); c->note = strdup(note); }
        } else if (strcmp(cmd, "MERGE") == 0) {
            if (sscanf(line, "%*s %255s %255s", n1, n2) != 2) continue;
            Contact *c1 = find_contact(n1), *c2 = find_contact(n2);
            if (c1 && c2) {
                char *new_note = malloc(strlen(c1->note) + strlen(c2->note) + 2);
                sprintf(new_note, "%s %s", c1->note, c2->note);
                free(c1->note); c1->note = new_note;
                
                Contact *prev = NULL, *curr = head;
                while (curr) {
                    if (curr == c2) {
                        if (prev) prev->next = curr->next; else head = curr->next;
                        free(curr->name); free(curr->email); free(curr->phone); free(curr->note); free(curr);
                        break;
                    }
                    prev = curr; curr = curr->next;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", n1) != 1) continue;
            Contact *prev = NULL, *curr = head;
            while (curr) {
                if (strcmp(curr->name, n1) == 0) {
                    Contact *tmp = curr; curr = curr->next;
                    if (prev) prev->next = curr; else head = curr;
                    free(tmp->name); free(tmp->email); free(tmp->phone); free(tmp->note); free(tmp);
                    break;
                }
                prev = curr; curr = curr->next;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(line, "%*s %255s", n1) != 1) continue;
            Contact *c = find_contact(n1);
            if (c) printf("%s %s %s %s\n", c->name, c->email, c->phone, c->note);
        } else if (strcmp(cmd, "LIST") == 0) {
            for (Contact *c = head; c; c = c->next)
                printf("%s %s %s %s\n", c->name, c->email, c->phone, c->note);
        }
    }
    free_list();
    return 0;
}