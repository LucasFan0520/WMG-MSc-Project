// F015.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Contact { char *name; char *email; char *phone; char *note; struct Contact *next; } Contact;
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
    c->name = strdup(name); c->email = strdup(email); c->phone = strdup(phone); c->note = strdup("");
    c->next = contacts; contacts = c;
}

void note_contact(const char *name, const char *text) {
    Contact *c = find_contact(name);
    if (!c) return;
    free(c->note); c->note = strdup(text);
}

void merge_contact(const char *keep, const char *remove) {
    Contact *k = find_contact(keep), *r = find_contact(remove);
    if (!k || !r) return;
    // combine notes: append remove's note
    size_t newlen = strlen(k->note) + strlen(r->note) + 2;
    char *buf = malloc(newlen);
    if (!buf) return;
    sprintf(buf, "%s %s", k->note, r->note);
    free(k->note); k->note = buf;
    // delete remove
    Contact *prev = NULL;
    for (Contact *c = contacts; c; prev = c, c = c->next) {
        if (c == r) {
            if (prev) prev->next = c->next; else contacts = c->next;
            free(c->name); free(c->email); free(c->phone); free(c->note); free(c);
            return;
        }
    }
}

void delete_contact(const char *name) {
    Contact *prev = NULL;
    for (Contact *c = contacts; c; prev = c, c = c->next) {
        if (strcmp(c->name, name) == 0) {
            if (prev) prev->next = c->next; else contacts = c->next;
            free(c->name); free(c->email); free(c->phone); free(c->note); free(c);
            return;
        }
    }
}

void find_contact_print(const char *name) {
    Contact *c = find_contact(name);
    if (!c) { printf("NOT_FOUND\n"); return; }
    printf("%s %s %s\n", c->email, c->phone, c->note);
}

void list_contacts(void) {
    for (Contact *c = contacts; c; c = c->next)
        printf("%s %s %s %s\n", c->name, c->email, c->phone, c->note);
}

int main() {
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
            if (sscanf(rest, "%255s %255s %255s", name, email, phone) == 3)
                add_contact(name, email, phone);
        } else if (strcmp(cmd, "NOTE") == 0) {
            char name[256], text[4096];
            if (sscanf(rest, "%255s", name) == 1) {
                char *t = rest + strlen(name);
                while (*t == ' ') t++;
                strcpy(text, t);
                note_contact(name, text);
            }
        } else if (strcmp(cmd, "MERGE") == 0) {
            char keep[256], remove[256];
            if (sscanf(rest, "%255s %255s", keep, remove) == 2) merge_contact(keep, remove);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char name[256];
            if (sscanf(rest, "%255s", name) == 1) delete_contact(name);
        } else if (strcmp(cmd, "FIND") == 0) {
            char name[256];
            if (sscanf(rest, "%255s", name) == 1) find_contact_print(name);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_contacts();
        }
    }
    while (contacts) { Contact *c = contacts; contacts = c->next; free(c->name); free(c->email); free(c->phone); free(c->note); free(c); }
    return 0;
}