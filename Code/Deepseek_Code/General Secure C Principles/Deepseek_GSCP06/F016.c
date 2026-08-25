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
    if (find_contact(name)) return;
    Contact *c = malloc(sizeof(Contact));
    c->name = strdup(name);
    c->email = strdup(email);
    c->phone = strdup(phone);
    c->next = contacts;
    contacts = c;
}

void delete_contact(const char *name) {
    Contact *c = find_contact(name);
    if (!c) return;
    Contact *cur = contacts, *prev = NULL;
    while (cur) {
        if (cur == c) {
            if (prev) prev->next = cur->next;
            else contacts = cur->next;
            break;
        }
        prev = cur;
        cur = cur->next;
    }
    free(c->name);
    free(c->email);
    free(c->phone);
    free(c);
}

void find_contact_cmd(const char *name) {
    Contact *c = find_contact(name);
    if (!c) return;
    printf("%s %s %s\n", c->name, c->email, c->phone);
}

void report_contacts(void) {
    Contact *c = contacts;
    while (c) {
        printf("%s %s %s\n", c->name, c->email, c->phone);
        c = c->next;
    }
}

void cleanup_contacts(void) {
    Contact *c = contacts;
    while (c) {
        Contact *tmp = c;
        c = c->next;
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
                char name[256], email[256], phone[256];
                if (sscanf(line, "%255[^,],%255[^,],%255[^\n]", name, email, phone) == 3) {
                    add_contact(name, email, phone);
                }
            }
            fclose(f);
        }
    }
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], arg1[256], arg2[256], arg3[256];
        int n = sscanf(line, "%31s %255s %255s %255s", cmd, arg1, arg2, arg3);
        if (n < 2) continue;
        if (strcmp(cmd, "ADD") == 0 && n == 4) {
            add_contact(arg1, arg2, arg3);
        } else if (strcmp(cmd, "DELETE") == 0) {
            delete_contact(arg1);
        } else if (strcmp(cmd, "FIND") == 0) {
            find_contact_cmd(arg1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_contacts();
        }
    }
    cleanup_contacts();
    return 0;
}