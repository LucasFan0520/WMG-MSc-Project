// F016.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Contact { char *name, *email, *phone; struct Contact *next; } Contact;
Contact *head = NULL;

void add_contact(const char *name, const char *email, const char *phone) {
    Contact *c = malloc(sizeof(Contact));
    c->name = strdup(name);
    c->email = strdup(email);
    c->phone = strdup(phone);
    c->next = head;
    head = c;
}

void delete_contact(const char *name) {
    Contact **p = &head;
    while (*p) {
        if (strcmp((*p)->name, name) == 0) {
            Contact *tmp = *p;
            *p = tmp->next;
            free(tmp->name);
            free(tmp->email);
            free(tmp->phone);
            free(tmp);
            return;
        }
        p = &(*p)->next;
    }
}

void find_contact(const char *name) {
    for (Contact *c = head; c; c = c->next) {
        if (strcmp(c->name, name) == 0) {
            printf("%s,%s,%s\n", c->name, c->email, c->phone);
            return;
        }
    }
}

void report() {
    for (Contact *c = head; c; c = c->next) printf("%s,%s,%s\n", c->name, c->email, c->phone);
}

void load_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
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

void cleanup() {
    while (head) {
        Contact *tmp = head->next;
        free(head->name);
        free(head->email);
        free(head->phone);
        free(head);
        head = tmp;
    }
}

int main(int argc, char *argv[]) {
    if (argc > 1) load_file(argv[1]);
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *t = strchr(line, ' ');
            if (t) {
                t++;
                char *name = strtok(t, ",");
                char *email = strtok(NULL, ",");
                char *phone = strtok(NULL, ",");
                if (name && email && phone) add_contact(name, email, phone);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char a1[256];
            if (sscanf(line, "%*s %255s", a1) == 1) delete_contact(a1);
        } else if (strcmp(cmd, "FIND") == 0) {
            char a1[256];
            if (sscanf(line, "%*s %255s", a1) == 1) find_contact(a1);
        } else if (strcmp(cmd, "REPORT") == 0) report();
    }
    cleanup();
    return 0;
}