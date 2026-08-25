/* F016.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Contact {
    char *name;
    char *email;
    char *phone;
    struct Contact *next;
} Contact;

Contact *contacts = NULL;

Contact *find_contact(const char *name) {
    Contact *c = contacts;
    while (c) {
        if (strcmp(c->name, name) == 0) return c;
        c = c->next;
    }
    return NULL;
}

void add_contact(const char *name, const char *email, const char *phone) {
    // if exists, we could update; but spec says add, we'll add new.
    Contact *c = malloc(sizeof(Contact));
    c->name = strdup(name);
    c->email = strdup(email);
    c->phone = strdup(phone);
    c->next = contacts;
    contacts = c;
}

void delete_contact(const char *name) {
    Contact *prev = NULL, *cur = contacts;
    while (cur) {
        if (strcmp(cur->name, name) == 0) {
            if (prev) prev->next = cur->next;
            else contacts = cur->next;
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

void print_contact(Contact *c) {
    printf("%s %s %s\n", c->name, c->email, c->phone);
}

void report_contacts(void) {
    Contact *c = contacts;
    while (c) {
        printf("%s\n", c->name);
        c = c->next;
    }
}

void free_contacts(void) {
    while (contacts) {
        Contact *tmp = contacts;
        contacts = contacts->next;
        free(tmp->name);
        free(tmp->email);
        free(tmp->phone);
        free(tmp);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) return 1;
    FILE *f = fopen(argv[1], "r");
    if (f) {
        char line[4096];
        while (fgets(line, sizeof(line), f)) {
            line[strcspn(line, "\n")] = '\0';
            char name[256], email[256], phone[256];
            if (sscanf(line, "%[^,],%[^,],%s", name, email, phone) == 3) {
                add_contact(name, email, phone);
            }
        }
        fclose(f);
    }

    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        char name[256], email[256], phone[256];
        int n = sscanf(line, "%s %s %s %s", cmd, name, email, phone);
        if (strcmp(cmd, "ADD") == 0) {
            if (n < 4) continue;
            add_contact(name, email, phone);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (n < 2) continue;
            delete_contact(name);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (n < 2) continue;
            Contact *c = find_contact(name);
            if (c) print_contact(c);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_contacts();
        }
    }
    free_contacts();
    return 0;
}