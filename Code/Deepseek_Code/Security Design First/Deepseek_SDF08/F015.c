// F015.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

typedef struct contact {
    char *name;
    char *email;
    char *phone;
    char *note;
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
    c->note = strdup("");
    if (!c->name || !c->email || !c->phone || !c->note) { fprintf(stderr, "Memory error\n"); exit(1); }
    c->next = contacts;
    contacts = c;
}

void note_contact(const char *name, const char *text) {
    Contact *c = find_contact(name);
    if (!c) return;
    free(c->note);
    c->note = strdup(text);
    if (!c->note) { fprintf(stderr, "Memory error\n"); exit(1); }
}

void merge_contacts(const char *keep, const char *remove) {
    Contact *c1 = find_contact(keep);
    Contact *c2 = find_contact(remove);
    if (!c1 || !c2) return;
    // Combine notes: c1 note + space + c2 note
    size_t new_len = strlen(c1->note) + strlen(c2->note) + 2;
    char *new_note = malloc(new_len);
    if (!new_note) { fprintf(stderr, "Memory error\n"); exit(1); }
    snprintf(new_note, new_len, "%s %s", c1->note, c2->note);
    free(c1->note);
    c1->note = new_note;
    // Delete remove contact
    Contact *prev = NULL, *cur = contacts;
    while (cur) {
        if (cur == c2) {
            if (prev) prev->next = cur->next;
            else contacts = cur->next;
            free(cur->name);
            free(cur->email);
            free(cur->phone);
            free(cur->note);
            free(cur);
            break;
        }
        prev = cur;
        cur = cur->next;
    }
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
            free(c->note);
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
    printf("%s %s %s ", c->name, c->email, c->phone);
    for (char *s = c->note; *s; s++) {
        putchar(*s == ' ' ? '_' : *s);
    }
    putchar('\n');
}

void list_contacts(void) {
    Contact *c = contacts;
    while (c) {
        printf("%s %s %s ", c->name, c->email, c->phone);
        for (char *s = c->note; *s; s++) {
            putchar(*s == ' ' ? '_' : *s);
        }
        putchar('\n');
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
        free(c->note);
        free(c);
        c = next;
    }
}

int main(void) {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        trim_newline(line);
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        if (sscanf(line, "%31s", cmd) < 1) continue;
        if (strcmp(cmd, "LIST") == 0) { list_contacts(); continue; }
        char *rest = line + strlen(cmd);
        while (isspace(*rest)) rest++;
        if (strcmp(cmd, "ADD") == 0) {
            char *name = rest;
            while (*rest && !isspace(*rest)) rest++;
            if (!*rest) continue;
            *rest = '\0';
            rest++;
            while (isspace(*rest)) rest++;
            char *email = rest;
            while (*rest && !isspace(*rest)) rest++;
            if (!*rest) continue;
            *rest = '\0';
            rest++;
            while (isspace(*rest)) rest++;
            char *phone = rest;
            while (*rest && !isspace(*rest)) rest++;
            if (!*rest) continue;
            *rest = '\0';
            add_contact(name, email, phone);
        } else if (strcmp(cmd, "NOTE") == 0) {
            char *name = rest;
            while (*rest && !isspace(*rest)) rest++;
            if (!*rest) continue;
            *rest = '\0';
            rest++;
            while (isspace(*rest)) rest++;
            note_contact(name, rest);
        } else if (strcmp(cmd, "MERGE") == 0) {
            char *keep = rest;
            while (*rest && !isspace(*rest)) rest++;
            if (!*rest) continue;
            *rest = '\0';
            rest++;
            while (isspace(*rest)) rest++;
            char *remove = rest;
            while (*rest && !isspace(*rest)) rest++;
            if (!*rest) continue;
            *rest = '\0';
            merge_contacts(keep, remove);
        } else if (strcmp(cmd, "DELETE") == 0) {
            delete_contact(rest);
        } else if (strcmp(cmd, "FIND") == 0) {
            find_contact_print(rest);
        }
    }
    free_all();
    return 0;
}