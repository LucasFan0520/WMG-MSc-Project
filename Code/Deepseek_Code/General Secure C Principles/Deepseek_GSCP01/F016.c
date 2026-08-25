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
    if (find_contact(name)) return;
    Contact *c = malloc(sizeof(Contact));
    c->name = strdup(name);
    c->email = strdup(email);
    c->phone = strdup(phone);
    c->next = contacts;
    contacts = c;
}

void delete_contact(const char *name) {
    Contact *c = contacts, *prev = NULL;
    while (c && strcmp(c->name, name) != 0) {
        prev = c;
        c = c->next;
    }
    if (!c) return;
    if (prev) prev->next = c->next;
    else contacts = c->next;
    free(c->name);
    free(c->email);
    free(c->phone);
    free(c);
}

void find_contact_print(const char *name) {
    Contact *c = find_contact(name);
    if (!c) { printf("NOT_FOUND\n"); return; }
    printf("%s %s %s\n", c->name, c->email, c->phone);
}

void report_all(void) {
    Contact *c = contacts;
    while (c) {
        printf("%s %s %s\n", c->name, c->email, c->phone);
        c = c->next;
    }
}

void free_all(void) {
    Contact *c = contacts;
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
    if (argc < 2) return 1;
    FILE *f = fopen(argv[1], "r");
    if (f) {
        char line[1024];
        while (fgets(line, sizeof(line), f)) {
            line[strcspn(line, "\n")] = '\0';
            char name[256], email[256], phone[256];
            if (sscanf(line, "%255[^,],%255[^,],%255[^,]", name, email, phone) == 3) {
                add_contact(name, email, phone);
            }
        }
        fclose(f);
    }
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char name[256], email[256], phone[256];
            if (sscanf(line + 4, "%255[^,],%255[^,],%255[^,]", name, email, phone) == 3)
                add_contact(name, email, phone);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            delete_contact(p);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            find_contact_print(p);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_all();
        }
    }
    free_all();
    free(line);
    return 0;
}