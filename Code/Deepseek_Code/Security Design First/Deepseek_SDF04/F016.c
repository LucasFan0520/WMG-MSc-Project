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

Contact *find_contact(const char *name) {
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
    if (!c) return;
    c->name = strdup(name);
    c->email = strdup(email);
    c->phone = strdup(phone);
    c->next = NULL;
    if (!c->name || !c->email || !c->phone) {
        free(c->name); free(c->email); free(c->phone); free(c);
        return;
    }
    if (!head) head = c;
    else {
        Contact *last = head;
        while (last->next) last = last->next;
        last->next = c;
    }
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

void report_contacts(void) {
    Contact *c = head;
    while (c) {
        printf("%s %s %s\n", c->name, c->email, c->phone);
        c = c->next;
    }
}

void free_all(void) {
    Contact *c = head;
    while (c) {
        Contact *next = c->next;
        free(c->name);
        free(c->email);
        free(c->phone);
        free(c);
        c = next;
    }
}

int main(int argc, char **argv) {
    if (argc >= 2) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char *line = NULL;
            size_t len = 0;
            while (getline(&line, &len, f) != -1) {
                line[strcspn(line, "\n")] = '\0';
                char name[256], email[256], phone[256];
                if (sscanf(line, "%255[^,],%255[^,],%255[^\n]", name, email, phone) == 3) {
                    add_contact(name, email, phone);
                }
            }
            free(line);
            fclose(f);
        }
    }

    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[64], name[256], email[256], phone[256];
        if (sscanf(line, "%63s", cmd) != 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%*s %255[^,],%255[^,],%255[^\n]", name, email, phone) == 3)
                add_contact(name, email, phone);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1)
                delete_contact(name);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1)
                find_contact_print(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_contacts();
        }
    }
    free(line);
    free_all();
    return 0;
}