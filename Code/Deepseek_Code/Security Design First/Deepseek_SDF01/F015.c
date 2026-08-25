// filename: F015.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Contact {
    char *name;
    char *email;
    char *phone;
    char *note;
    struct Contact *next;
} Contact;

static Contact *contacts = NULL;

static void cleanup(void) {
    while (contacts) {
        Contact *tmp = contacts;
        contacts = contacts->next;
        free(tmp->name);
        free(tmp->email);
        free(tmp->phone);
        free(tmp->note);
        free(tmp);
    }
}

static Contact *find_contact(const char *name) {
    for (Contact *c = contacts; c; c = c->next)
        if (strcmp(c->name, name) == 0)
            return c;
    return NULL;
}

static void add_contact(const char *name, const char *email, const char *phone) {
    if (find_contact(name)) return;
    Contact *c = malloc(sizeof(Contact));
    if (!c) return;
    c->name = strdup(name);
    c->email = strdup(email);
    c->phone = strdup(phone);
    c->note = strdup(""); // empty note
    if (!c->name || !c->email || !c->phone || !c->note) {
        free(c->name);
        free(c->email);
        free(c->phone);
        free(c->note);
        free(c);
        return;
    }
    c->next = contacts;
    contacts = c;
}

static void set_note(const char *name, const char *note) {
    Contact *c = find_contact(name);
    if (!c) return;
    char *new = strdup(note);
    if (!new) return;
    free(c->note);
    c->note = new;
}

static void merge_contacts(const char *keep, const char *remove) {
    Contact *k = find_contact(keep);
    Contact *r = find_contact(remove);
    if (!k || !r) return;
    // combine notes: k's note + " " + r's note
    size_t len = strlen(k->note) + strlen(r->note) + 2;
    char *combined = malloc(len);
    if (!combined) return;
    snprintf(combined, len, "%s %s", k->note, r->note);
    free(k->note);
    k->note = combined;
    // delete removed contact
    Contact *prev = NULL;
    for (Contact *c = contacts; c; c = c->next) {
        if (c == r) {
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
    }
}

static void delete_contact(const char *name) {
    Contact *prev = NULL;
    for (Contact *c = contacts; c; c = c->next) {
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
    }
}

static void find_print(const char *name) {
    Contact *c = find_contact(name);
    if (!c) return;
    printf("%s %s %s %s\n", c->name, c->email, c->phone, c->note);
}

static void list_contacts(void) {
    for (Contact *c = contacts; c; c = c->next)
        printf("%s %s %s %s\n", c->name, c->email, c->phone, c->note);
}

int main(void) {
    char *line = NULL;
    size_t linelen = 0;
    atexit(cleanup);
    while (getline(&line, &linelen, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *p = line;
        while (*p == ' ') ++p;
        char *cmd = p;
        while (*p && *p != ' ') ++p;
        if (*p) *p++ = '\0';
        while (*p == ' ') ++p;
        char *arg1 = p;
        while (*p && *p != ' ') ++p;
        if (*p) *p++ = '\0';
        while (*p == ' ') ++p;
        char *arg2 = p;
        while (*p && *p != ' ') ++p;
        if (*p) *p++ = '\0';
        while (*p == ' ') ++p;
        char *arg3 = p;
        if (strcmp(cmd, "ADD") == 0) {
            if (*arg1 && *arg2) add_contact(arg1, arg2, arg3);
        } else if (strcmp(cmd, "NOTE") == 0) {
            if (*arg1) set_note(arg1, arg2);
        } else if (strcmp(cmd, "MERGE") == 0) {
            if (*arg1 && *arg2) merge_contacts(arg1, arg2);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (*arg1) delete_contact(arg1);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (*arg1) find_print(arg1);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_contacts();
        }
    }
    free(line);
    return 0;
}