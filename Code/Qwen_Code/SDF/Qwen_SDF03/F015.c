// F015.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Contact {
    char *name, *email, *phone, *note;
    struct Contact *next;
} Contact;

Contact *head = NULL;

char *get_token(char **s) {
    while (**s == ' ') (*s)++;
    if (!**s) return NULL;
    char *start = *s;
    while (**s && **s != ' ') (*s)++;
    if (**s) { **s = '\0'; (*s)++; }
    return start;
}

Contact *find_contact(const char *name) {
    for (Contact *c = head; c; c = c->next)
        if (strcmp(c->name, name) == 0) return c;
    return NULL;
}

int main() {
    char line[65536];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *ptr = line;
        char *cmd = get_token(&ptr);
        if (!cmd) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *name = get_token(&ptr);
            char *email = get_token(&ptr);
            char *phone = get_token(&ptr);
            if (name && email && phone && !find_contact(name)) {
                Contact *c = malloc(sizeof(Contact));
                if (c) { c->name = strdup(name); c->email = strdup(email); c->phone = strdup(phone); c->note = strdup(""); c->next = head; head = c; }
            }
        } else if (strcmp(cmd, "NOTE") == 0) {
            char *name = get_token(&ptr);
            if (name) {
                Contact *c = find_contact(name);
                if (c) { char *tmp = strdup(ptr); if (tmp) { free(c->note); c->note = tmp; } }
            }
        } else if (strcmp(cmd, "MERGE") == 0) {
            char *keep = get_token(&ptr);
            char *rem = get_token(&ptr);
            if (keep && rem) {
                Contact *ck = find_contact(keep), *cr = find_contact(rem);
                if (ck && cr) {
                    size_t len = strlen(ck->note) + strlen(cr->note) + 2;
                    char *buf = malloc(len);
                    if (buf) {
                        snprintf(buf, len, "%s %s", ck->note, cr->note);
                        free(ck->note); ck->note = buf;
                    }
                    Contact *prev = NULL, *curr = head;
                    while (curr) {
                        if (curr == cr) {
                            if (prev) prev->next = curr->next; else head = curr->next;
                            free(curr->name); free(curr->email); free(curr->phone); free(curr->note); free(curr);
                            break;
                        }
                        prev = curr; curr = curr->next;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = get_token(&ptr);
            if (name) {
                Contact *prev = NULL, *curr = head;
                while (curr) {
                    if (strcmp(curr->name, name) == 0) {
                        if (prev) prev->next = curr->next; else head = curr->next;
                        free(curr->name); free(curr->email); free(curr->phone); free(curr->note); free(curr); break;
                    }
                    prev = curr; curr = curr->next;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *name = get_token(&ptr);
            if (name) {
                Contact *c = find_contact(name);
                if (c) printf("%s %s %s %s\n", c->name, c->email, c->phone, c->note);
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (Contact *c = head; c; c = c->next) printf("%s %s %s %s\n", c->name, c->email, c->phone, c->note);
        }
    }
    while (head) {
        Contact *next = head->next;
        free(head->name); free(head->email); free(head->phone); free(head->note); free(head);
        head = next;
    }
    return 0;
}