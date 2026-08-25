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
    c->name = malloc(strlen(name) + 1);
    strcpy(c->name, name);
    c->email = malloc(strlen(email) + 1);
    strcpy(c->email, email);
    c->phone = malloc(strlen(phone) + 1);
    strcpy(c->phone, phone);
    c->next = head;
    head = c;
}

void delete_contact(const char *name) {
    Contact *prev = NULL;
    Contact *c = head;
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

void report_all(void) {
    Contact *c = head;
    while (c) {
        printf("%s %s %s\n", c->name, c->email, c->phone);
        c = c->next;
    }
}

void load_file(const char *fname) {
    FILE *f = fopen(fname, "r");
    if (!f) return;
    char line[4096];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = 0;
        // parse name,email,phone
        char *p = line;
        char *name = p;
        while (*p && *p != ',') p++;
        if (!*p) continue;
        *p++ = 0;
        char *email = p;
        while (*p && *p != ',') p++;
        if (!*p) continue;
        *p++ = 0;
        char *phone = p;
        // validate non-empty?
        if (strlen(name) > 0 && strlen(email) > 0 && strlen(phone) > 0) {
            add_contact(name, email, phone);
        }
    }
    fclose(f);
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
    if (argc > 1) {
        load_file(argv[1]);
    }
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ',') p++;
            if (*p) *p++ = 0;
            char *email = p;
            while (*p && *p != ',') p++;
            if (*p) *p++ = 0;
            char *phone = p;
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
    return 0;
}