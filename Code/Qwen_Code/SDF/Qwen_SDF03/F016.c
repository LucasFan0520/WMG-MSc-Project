// F016.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Contact {
    char *name, *email, *phone;
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

void parse_csv(char *line) {
    char *name = strtok(line, ",\n");
    char *email = strtok(NULL, ",\n");
    char *phone = strtok(NULL, ",\n");
    if (name && email && phone && !find_contact(name)) {
        Contact *c = malloc(sizeof(Contact));
        if (c) { c->name = strdup(name); c->email = strdup(email); c->phone = strdup(phone); c->next = head; head = c; }
    }
}

int main(int argc, char **argv) {
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char line[4096];
            while (fgets(line, sizeof(line), f)) parse_csv(line);
            fclose(f);
        }
    }
    char line[65536];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *ptr = line;
        char *cmd = get_token(&ptr);
        if (!cmd) continue;
        if (strcmp(cmd, "ADD") == 0) {
            parse_csv(ptr);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = get_token(&ptr);
            if (name) {
                Contact *prev = NULL, *curr = head;
                while (curr) {
                    if (strcmp(curr->name, name) == 0) {
                        if (prev) prev->next = curr->next; else head = curr->next;
                        free(curr->name); free(curr->email); free(curr->phone); free(curr); break;
                    }
                    prev = curr; curr = curr->next;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *name = get_token(&ptr);
            if (name) {
                Contact *c = find_contact(name);
                if (c) printf("%s %s %s\n", c->name, c->email, c->phone);
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (Contact *c = head; c; c = c->next) printf("%s %s %s\n", c->name, c->email, c->phone);
        }
    }
    while (head) {
        Contact *next = head->next;
        free(head->name); free(head->email); free(head->phone); free(head);
        head = next;
    }
    return 0;
}