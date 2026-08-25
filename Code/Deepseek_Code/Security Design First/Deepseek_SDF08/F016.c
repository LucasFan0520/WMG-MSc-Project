// F016.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

typedef struct contact {
    char *name;
    char *email;
    char *phone;
    struct contact *next;
} Contact;

Contact *contacts = NULL;

void trim_newline(char *s) {
    size_t len = strlen(s);
    if (len > 0 && s[len-1] == '\n') s[len-1] = '\0';
}

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
    if (!c) { fprintf(stderr, "Memory error\n"); exit(1); }
    c->name = strdup(name);
    c->email = strdup(email);
    c->phone = strdup(phone);
    if (!c->name || !c->email || !c->phone) { fprintf(stderr, "Memory error\n"); exit(1); }
    c->next = contacts;
    contacts = c;
}

void delete_contact(const char *name) {
    Contact *c = contacts, *prev = NULL;
    while (c) {
        if (strcmp(c->name, name) == 0) {
            if (prev) prev->next = c->next;
            else contacts = c->next;
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

void load_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), f)) {
        trim_newline(line);
        char *p = line;
        char *name = p;
        while (*p && *p != ',') p++;
        if (*p != ',') continue;
        *p = '\0';
        p++;
        char *email = p;
        while (*p && *p != ',') p++;
        if (*p != ',') continue;
        *p = '\0';
        p++;
        char *phone = p;
        // remove trailing whitespace
        char *end = phone + strlen(phone) - 1;
        while (end > phone && isspace(*end)) { *end = '\0'; end--; }
        add_contact(name, email, phone);
    }
    fclose(f);
}

int main(int argc, char **argv) {
    if (argc > 1) {
        load_file(argv[1]);
    }
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        trim_newline(line);
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        if (sscanf(line, "%31s", cmd) < 1) continue;
        if (strcmp(cmd, "REPORT") == 0) { report_contacts(); continue; }
        char *rest = line + strlen(cmd);
        while (isspace(*rest)) rest++;
        if (strcmp(cmd, "ADD") == 0) {
            char *name = rest;
            while (*rest && *rest != ',') rest++;
            if (*rest != ',') continue;
            *rest = '\0';
            rest++;
            char *email = rest;
            while (*rest && *rest != ',') rest++;
            if (*rest != ',') continue;
            *rest = '\0';
            rest++;
            char *phone = rest;
            add_contact(name, email, phone);
        } else if (strcmp(cmd, "DELETE") == 0) {
            delete_contact(rest);
        } else if (strcmp(cmd, "FIND") == 0) {
            find_contact_print(rest);
        }
    }
    free_all();
    return 0;
}