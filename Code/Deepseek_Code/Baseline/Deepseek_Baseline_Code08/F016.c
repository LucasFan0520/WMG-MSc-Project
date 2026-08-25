// F016.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Contact {
    char *name;
    char *email;
    char *phone;
    struct Contact *next;
} Contact;

Contact *head = NULL;

Contact* find_contact(const char *name) {
    Contact *cur = head;
    while (cur) {
        if (strcmp(cur->name, name) == 0) return cur;
        cur = cur->next;
    }
    return NULL;
}

void add_contact(const char *name, const char *email, const char *phone) {
    if (find_contact(name)) return;
    Contact *new = malloc(sizeof(Contact));
    new->name = strdup(name);
    new->email = strdup(email);
    new->phone = strdup(phone);
    new->next = NULL;
    if (!head) head = new;
    else {
        Contact *cur = head;
        while (cur->next) cur = cur->next;
        cur->next = new;
    }
}

void delete_contact(const char *name) {
    Contact *prev = NULL, *cur = head;
    while (cur) {
        if (strcmp(cur->name, name) == 0) {
            if (prev) prev->next = cur->next;
            else head = cur->next;
            free(cur->name);
            free(cur->email);
            free(cur->phone);
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void print_contact(const char *name) {
    Contact *c = find_contact(name);
    if (!c) return;
    printf("%s %s %s\n", c->name, c->email, c->phone);
}

void print_all() {
    Contact *cur = head;
    while (cur) {
        printf("%s %s %s\n", cur->name, cur->email, cur->phone);
        cur = cur->next;
    }
}

void free_all() {
    Contact *cur = head;
    while (cur) {
        Contact *tmp = cur;
        cur = cur->next;
        free(tmp->name);
        free(tmp->email);
        free(tmp->phone);
        free(tmp);
    }
}

int main(int argc, char **argv) {
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char line[1024];
            while (fgets(line, sizeof(line), f)) {
                line[strcspn(line, "\n")] = 0;
                char *name = strtok(line, ",");
                if (!name) continue;
                char *email = strtok(NULL, ",");
                if (!email) continue;
                char *phone = strtok(NULL, ",");
                if (!phone) continue;
                add_contact(name, email, phone);
            }
            fclose(f);
        }
    }
    char cmdline[4096];
    while (fgets(cmdline, sizeof(cmdline), stdin)) {
        cmdline[strcspn(cmdline, "\n")] = 0;
        if (strcmp(cmdline, "END") == 0) break;
        char *cmd = strtok(cmdline, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *name = strtok(NULL, ",");
            if (!name) continue;
            char *email = strtok(NULL, ",");
            if (!email) continue;
            char *phone = strtok(NULL, ",");
            if (!phone) continue;
            add_contact(name, email, phone);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = strtok(NULL, " ");
            if (name) delete_contact(name);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *name = strtok(NULL, " ");
            if (name) print_contact(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            print_all();
        }
    }
    free_all();
    return 0;
}