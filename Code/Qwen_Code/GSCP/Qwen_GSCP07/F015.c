// F015.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Contact { char *name, *email, *phone, *note; struct Contact *next; } Contact;
Contact *head = NULL;

void add_contact(const char *name, const char *email, const char *phone) {
    Contact *c = malloc(sizeof(Contact));
    c->name = strdup(name);
    c->email = strdup(email);
    c->phone = strdup(phone);
    c->note = strdup("");
    c->next = head;
    head = c;
}

void note_contact(const char *name, const char *text) {
    for (Contact *c = head; c; c = c->next) {
        if (strcmp(c->name, name) == 0) {
            free(c->note);
            c->note = strdup(text);
            return;
        }
    }
}

void merge_contact(const char *keep, const char *remove) {
    Contact *ck = NULL, *cr = NULL;
    for (Contact *c = head; c; c = c->next) {
        if (strcmp(c->name, keep) == 0) ck = c;
        if (strcmp(c->name, remove) == 0) cr = c;
    }
    if (ck && cr) {
        size_t len = strlen(ck->note) + strlen(cr->note) + 2;
        char *buf = malloc(len);
        snprintf(buf, len, "%s %s", ck->note, cr->note);
        free(ck->note);
        ck->note = buf;
        
        Contact **p = &head;
        while (*p) {
            if (*p == cr) {
                *p = cr->next;
                free(cr->name);
                free(cr->email);
                free(cr->phone);
                free(cr->note);
                free(cr);
                break;
            }
            p = &(*p)->next;
        }
    }
}

void delete_contact(const char *name) {
    Contact **p = &head;
    while (*p) {
        if (strcmp((*p)->name, name) == 0) {
            Contact *tmp = *p;
            *p = tmp->next;
            free(tmp->name);
            free(tmp->email);
            free(tmp->phone);
            free(tmp->note);
            free(tmp);
            return;
        }
        p = &(*p)->next;
    }
}

void find_contact(const char *name) {
    for (Contact *c = head; c; c = c->next) {
        if (strcmp(c->name, name) == 0) {
            printf("%s %s %s %s\n", c->name, c->email, c->phone, c->note);
            return;
        }
    }
}

void list_contacts() {
    for (Contact *c = head; c; c = c->next) printf("%s %s %s %s\n", c->name, c->email, c->phone, c->note);
}

void cleanup() {
    while (head) {
        Contact *tmp = head->next;
        free(head->name);
        free(head->email);
        free(head->phone);
        free(head->note);
        free(head);
        head = tmp;
    }
}

int main() {
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], a1[256], a2[256], a3[256];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%*s %255s %255s %255s", a1, a2, a3) == 3) add_contact(a1, a2, a3);
        } else if (strcmp(cmd, "NOTE") == 0) {
            if (sscanf(line, "%*s %255s", a1) == 1) {
                char *t = strchr(line, ' ');
                if (t) { t = strchr(t + 1, ' '); if (t) note_contact(a1, t + 1); }
            }
        } else if (strcmp(cmd, "MERGE") == 0) {
            if (sscanf(line, "%*s %255s %255s", a1, a2) == 2) merge_contact(a1, a2);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", a1) == 1) delete_contact(a1);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(line, "%*s %255s", a1) == 1) find_contact(a1);
        } else if (strcmp(cmd, "LIST") == 0) list_contacts();
    }
    cleanup();
    return 0;
}