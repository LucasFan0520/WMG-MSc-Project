// F016.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Contact {
    char *name, *email, *phone;
    struct Contact *next;
} Contact;

Contact *head = NULL;

void free_list() {
    while (head) {
        Contact *t = head; head = head->next;
        free(t->name); free(t->email); free(t->phone); free(t);
    }
}

void add_contact(const char *name, const char *email, const char *phone) {
    Contact *c = malloc(sizeof(Contact));
    c->name = strdup(name); c->email = strdup(email); c->phone = strdup(phone);
    c->next = head; head = c;
}

Contact *find_contact(const char *name) {
    for (Contact *c = head; c; c = c->next)
        if (strcmp(c->name, name) == 0) return c;
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char line[1024];
            while (fgets(line, sizeof(line), f)) {
                line[strcspn(line, "\n")] = 0;
                char *name = strtok(line, ",");
                char *email = strtok(NULL, ",");
                char *phone = strtok(NULL, ",");
                if (name && email && phone) add_contact(name, email, phone);
            }
            fclose(f);
        }
    }

    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        char cmd[16], name[256], email[256], phone[256];
        if (sscanf(line, "%15s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;

        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%*s %255[^,],%255[^,],%255s", name, email, phone) != 3) continue;
            if (!find_contact(name)) add_contact(name, email, phone);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", name) != 1) continue;
            Contact *prev = NULL, *curr = head;
            while (curr) {
                if (strcmp(curr->name, name) == 0) {
                    Contact *tmp = curr; curr = curr->next;
                    if (prev) prev->next = curr; else head = curr;
                    free(tmp->name); free(tmp->email); free(tmp->phone); free(tmp);
                    break;
                }
                prev = curr; curr = curr->next;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(line, "%*s %255s", name) != 1) continue;
            Contact *c = find_contact(name);
            if (c) printf("%s,%s,%s\n", c->name, c->email, c->phone);
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (Contact *c = head; c; c = c->next)
                printf("%s,%s,%s\n", c->name, c->email, c->phone);
        }
    }
    free_list();
    return 0;
}