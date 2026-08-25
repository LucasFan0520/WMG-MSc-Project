/* F015.c */
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
    c->note = strdup("");
    c->next = contacts;
    contacts = c;
}

void note_contact(const char *name, const char *text) {
    Contact *c = find_contact(name);
    if (!c) return;
    free(c->note);
    c->note = strdup(text);
}

void merge_contacts(const char *keep, const char *remove) {
    Contact *k = find_contact(keep);
    Contact *r = find_contact(remove);
    if (!k || !r) return;
    char *new_note = malloc(strlen(k->note) + strlen(r->note) + 1);
    strcpy(new_note, k->note);
    strcat(new_note, r->note);
    free(k->note);
    k->note = new_note;
    delete_contact(remove);
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
    free(c->note);
    free(c);
}

void find_contact_print(const char *name) {
    Contact *c = find_contact(name);
    if (!c) { printf("NOT_FOUND\n"); return; }
    printf("%s %s %s ", c->name, c->email, c->phone);
    for (char *p = c->note; *p; p++) {
        putchar(*p == ' ' ? '_' : *p);
    }
    putchar('\n');
}

void list_contacts(void) {
    Contact *c = contacts;
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
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char name[256], email[256], phone[256];
            if (sscanf(line + 4, "%255s %255s %255s", name, email, phone) == 3)
                add_contact(name, email, phone);
        } else if (strcmp(cmd, "NOTE") == 0) {
            char name[256];
            char *p = line + 5;
            if (sscanf(p, "%255s", name) != 1) continue;
            while (*p && *p != ' ') p++;
            while (*p == ' ') p++;
            note_contact(name, p);
        } else if (strcmp(cmd, "MERGE") == 0) {
            char keep[256], remove[256];
            if (sscanf(line + 6, "%255s %255s", keep, remove) == 2)
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
    free(line);
    return 0;
}