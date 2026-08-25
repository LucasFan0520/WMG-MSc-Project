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

Contact *head = NULL;

Contact* find_contact(const char *name) {
    Contact *c = head;
    while (c) {
        if (strcmp(c->name, name) == 0) return c;
        c = c->next;
    }
    return NULL;
}

void add_contact(const char *name, const char *email, const char *phone) {
    if (find_contact(name)) return;
    Contact *c = malloc(sizeof(Contact));
    c->name = strdup(name);
    c->email = strdup(email);
    c->phone = strdup(phone);
    c->next = head;
    head = c;
}

void delete_contact(const char *name) {
    Contact *c = head, *prev = NULL;
    while (c) {
        if (strcmp(c->name, name) == 0) {
            if (prev) prev->next = c->next;
            else head = c->next;
            free(c->name);
            free(c->email);
            free(c->phone);
            free(c);
            return;
        }
        prev = c;
        c = c->next;
    }
}

void find_contact_print(const char *name) {
    Contact *c = find_contact(name);
    if (!c) return;
    printf("%s %s %s\n", c->name, c->email, c->phone);
}

void report_contacts() {
    Contact *c = head;
    while (c) {
        printf("%s %s %s\n", c->name, c->email, c->phone);
        c = c->next;
    }
}

void load_file(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) return;
    char line[4096];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = '\0';
        char name[256], email[256], phone[256];
        if (sscanf(line, "%255[^,],%255[^,],%255[^\n]", name, email, phone) == 3) {
            add_contact(name, email, phone);
        }
    }
    fclose(f);
}

void free_all() {
    while (head) {
        Contact *tmp = head;
        head = head->next;
        free(tmp->name);
        free(tmp->email);
        free(tmp->phone);
        free(tmp);
    }
}

int main(int argc, char **argv) {
    if (argc > 1) load_file(argv[1]);
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32]; char name[256], email[256], phone[256];
        int n = sscanf(line, "%31s %255[^,],%255[^,],%255[^\n]", cmd, name, email, phone);
        if (strcmp(cmd, "ADD") == 0 && n == 4) {
            add_contact(name, email, phone);
        } else if (strcmp(cmd, "DELETE") == 0 && n == 2) {
            delete_contact(name);
        } else if (strcmp(cmd, "FIND") == 0 && n == 2) {
            find_contact_print(name);
        } else if (strcmp(cmd, "REPORT") == 0 && n == 1) {
            report_contacts();
        }
    }
    free_all();
    return 0;
}