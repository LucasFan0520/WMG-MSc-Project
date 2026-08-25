// F015.c
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
    c->note = malloc(1);
    c->note[0] = 0;
    c->next = head;
    head = c;
}

void note_contact(const char *name, const char *note) {
    Contact *c = find_contact(name);
    if (!c) return;
    free(c->note);
    c->note = malloc(strlen(note) + 1);
    strcpy(c->note, note);
}

void merge_contacts(const char *keep, const char *remove) {
    Contact *k = find_contact(keep);
    Contact *r = find_contact(remove);
    if (!k || !r) return;
    // combine note fields: keep note + space + remove note
    char *new_note = malloc(strlen(k->note) + strlen(r->note) + 2);
    strcpy(new_note, k->note);
    if (strlen(k->note) > 0 && strlen(r->note) > 0) strcat(new_note, " ");
    strcat(new_note, r->note);
    free(k->note);
    k->note = new_note;
    // delete remove contact
    Contact *prev = NULL;
    Contact *c = head;
    while (c) {
        if (c == r) {
            if (prev) prev->next = c->next;
            else head = c->next;
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
    for (char *p = c->note; *p; p++) {
        putchar(*p == ' ' ? '_' : *p);
    }
    putchar('\n');
}

void list_contacts(void) {
    Contact *c = head;
    while (c) {
        printf("%s %s %s ", c->name, c->email, c->phone);
        for (char *p = c->note; *p; p++) {
            putchar(*p == ' ' ? '_' : *p);
        }
        putchar('\n');
        c = c->next;
    }
}

void free_all(void) {
    Contact *c = head;
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

int main() {
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
            while (*p && *p != ' ') p++;
            if (*p) *p++ = 0;
            while (*p == ' ') p++;
            char *email = p;
            while (*p && *p != ' ') p++;
            if (*p) *p++ = 0;
            while (*p == ' ') p++;
            char *phone = p;
            // note is not set yet
            add_contact(name, email, phone);
        } else if (strcmp(cmd, "NOTE") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) *p++ = 0;
            while (*p == ' ') p++;
            note_contact(name, p);
        } else if (strcmp(cmd, "MERGE") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            char *keep = p;
            while (*p && *p != ' ') p++;
            if (*p) *p++ = 0;
            while (*p == ' ') p++;
            char *remove = p;
            merge_contacts(keep, remove);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            delete_contact(p);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            find_contact_print(p);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_contacts();
        }
    }
    free_all();
    return 0;
}