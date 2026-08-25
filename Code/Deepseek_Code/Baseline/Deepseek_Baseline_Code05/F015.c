/* F015.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct contact {
    char *name;
    char *email;
    char *phone;
    char *note;
    struct contact *next;
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
    Contact *c = find_contact(name);
    if (c) {
        free(c->email); free(c->phone);
        c->email = strdup(email);
        c->phone = strdup(phone);
        return;
    }
    Contact *new = malloc(sizeof(Contact));
    new->name = strdup(name);
    new->email = strdup(email);
    new->phone = strdup(phone);
    new->note = NULL;
    new->next = head;
    head = new;
}

void note_contact(const char *name, const char *text) {
    Contact *c = find_contact(name);
    if (!c) return;
    if (c->note) free(c->note);
    c->note = strdup(text);
}

void merge_contacts(const char *keep, const char *remove) {
    Contact *k = find_contact(keep);
    Contact *r = find_contact(remove);
    if (!k || !r) return;
    // combine note fields
    if (r->note) {
        if (k->note) {
            char *combined = malloc(strlen(k->note) + strlen(r->note) + 2);
            sprintf(combined, "%s %s", k->note, r->note);
            free(k->note);
            k->note = combined;
        } else {
            k->note = strdup(r->note);
        }
    }
    // delete removed contact
    Contact *p = head, *prev = NULL;
    while (p) {
        if (p == r) {
            if (prev) prev->next = p->next;
            else head = p->next;
            free(p->name); free(p->email); free(p->phone);
            if (p->note) free(p->note);
            free(p);
            return;
        }
        prev = p;
        p = p->next;
    }
}

void delete_contact(const char *name) {
    Contact *c = head, *prev = NULL;
    while (c) {
        if (strcmp(c->name, name) == 0) {
            if (prev) prev->next = c->next;
            else head = c->next;
            free(c->name); free(c->email); free(c->phone);
            if (c->note) free(c->note);
            free(c);
            return;
        }
        prev = c;
        c = c->next;
    }
}

void find_contact_print(const char *name) {
    Contact *c = find_contact(name);
    if (!c) { printf("NOT_FOUND\n"); return; }
    printf("%s %s %s ", c->name, c->email, c->phone);
    if (c->note) {
        for (char *s = c->note; *s; s++) putchar(*s == ' ' ? '_' : *s);
    }
    putchar('\n');
}

void list_contacts() {
    Contact *c = head;
    while (c) {
        printf("%s %s %s ", c->name, c->email, c->phone);
        if (c->note) {
            for (char *s = c->note; *s; s++) putchar(*s == ' ' ? '_' : *s);
        }
        putchar('\n');
        c = c->next;
    }
}

int main() {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], name[256], name2[256], text[1024];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "LIST") == 0) { list_contacts(); continue; }
        if (strcmp(cmd, "ADD") == 0) {
            char email[256], phone[256];
            if (sscanf(line, "%*s %255s %255s %255s", name, email, phone) == 3)
                add_contact(name, email, phone);
        } else if (strcmp(cmd, "NOTE") == 0) {
            if (sscanf(line, "%*s %255s", name) < 1) continue;
            char *rest = line + strlen(cmd) + 1 + strlen(name) + 1;
            if (*rest == ' ') rest++;
            note_contact(name, rest);
        } else if (strcmp(cmd, "MERGE") == 0) {
            if (sscanf(line, "%*s %255s %255s", name, name2) == 2)
                merge_contacts(name, name2);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1) delete_contact(name);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1) find_contact_print(name);
        }
    }
    return 0;
}