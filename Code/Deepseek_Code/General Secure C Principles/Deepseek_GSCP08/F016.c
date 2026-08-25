// F016.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Contact { char *name; char *email; char *phone; struct Contact *next; } Contact;
Contact *contacts = NULL;

Contact *find_contact(const char *name) {
    for (Contact *c = contacts; c; c = c->next)
        if (strcmp(c->name, name) == 0) return c;
    return NULL;
}

void add_contact(const char *name, const char *email, const char *phone) {
    if (find_contact(name)) return;
    Contact *c = malloc(sizeof(Contact));
    if (!c) return;
    c->name = strdup(name); c->email = strdup(email); c->phone = strdup(phone);
    c->next = contacts; contacts = c;
}

void delete_contact(const char *name) {
    Contact *prev = NULL;
    for (Contact *c = contacts; c; prev = c, c = c->next) {
        if (strcmp(c->name, name) == 0) {
            if (prev) prev->next = c->next; else contacts = c->next;
            free(c->name); free(c->email); free(c->phone); free(c);
            return;
        }
    }
}

void find_contact_print(const char *name) {
    Contact *c = find_contact(name);
    if (!c) { printf("NOT_FOUND\n"); return; }
    printf("%s %s\n", c->email, c->phone);
}

void report_contacts(void) {
    for (Contact *c = contacts; c; c = c->next)
        printf("%s %s %s\n", c->name, c->email, c->phone);
}

int main(int argc, char **argv) {
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char line[4096];
            while (fgets(line, sizeof(line), f)) {
                line[strcspn(line, "\n")] = 0;
                char name[256], email[256], phone[256];
                if (sscanf(line, "%255[^,],%255[^,],%255s", name, email, phone) == 3)
                    add_contact(name, email, phone);
            }
            fclose(f);
        }
    }
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) != 1) continue;
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "ADD") == 0) {
            char name[256], email[256], phone[256];
            if (sscanf(rest, "%255[^,],%255[^,],%255s", name, email, phone) == 3)
                add_contact(name, email, phone);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char name[256];
            if (sscanf(rest, "%255s", name) == 1) delete_contact(name);
        } else if (strcmp(cmd, "FIND") == 0) {
            char name[256];
            if (sscanf(rest, "%255s", name) == 1) find_contact_print(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_contacts();
        }
    }
    while (contacts) { Contact *c = contacts; contacts = c->next; free(c->name); free(c->email); free(c->phone); free(c); }
    return 0;
}