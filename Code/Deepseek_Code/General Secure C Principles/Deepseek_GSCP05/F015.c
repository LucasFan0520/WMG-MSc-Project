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
    if (!c) return;
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
    Contact *kc = find_contact(keep);
    Contact *rc = find_contact(remove);
    if (!kc || !rc) return;
    // combine notes: keep_note + " " + remove_note
    char *new_note = malloc(strlen(kc->note) + strlen(rc->note) + 2);
    if (!new_note) return;
    sprintf(new_note, "%s %s", kc->note, rc->note);
    free(kc->note);
    kc->note = new_note;
    // delete remove contact
    Contact *prev = NULL, *cur = contacts;
    while (cur) {
        if (cur == rc) {
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
    Contact *prev = NULL, *cur = contacts;
    while (cur) {
        if (strcmp(cur->name, name) == 0) {
            if (prev) prev->next = cur->next;
            else contacts = cur->next;
            free(cur->name);
            free(cur->email);
            free(cur->phone);
            free(cur->note);
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void find_contact_print(const char *name) {
    Contact *c = find_contact(name);
    if (!c) return;
    printf("%s %s %s: ", c->name, c->email, c->phone);
    char *s = c->note;
    for (; *s; s++) putchar(*s == ' ' ? '_' : *s);
    putchar('\n');
}

void list_contacts(void) {
    Contact *c = contacts;
    while (c) {
        printf("%s %s %s: ", c->name, c->email, c->phone);
        char *s = c->note;
        for (; *s; s++) putchar(*s == ' ' ? '_' : *s);
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
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) != 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *name = p;
            char *space = strchr(name, ' ');
            if (!space) continue;
            *space = '\0';
            char *email = space + 1;
            while (*email == ' ') email++;
            space = strchr(email, ' ');
            if (!space) continue;
            *space = '\0';
            char *phone = space + 1;
            while (*phone == ' ') phone++;
            add_contact(name, email, phone);
        } else if (strcmp(cmd, "NOTE") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            char *name = p;
            char *space = strchr(name, ' ');
            if (!space) continue;
            *space = '\0';
            char *text = space + 1;
            while (*text == ' ') text++;
            note_contact(name, text);
        } else if (strcmp(cmd, "MERGE") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            char *keep = p;
            char *space = strchr(keep, ' ');
            if (!space) continue;
            *space = '\0';
            char *remove = space + 1;
            while (*remove == ' ') remove++;
            merge_contacts(keep, remove);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *name = p;
            char *space = strchr(name, ' ');
            if (space) *space = '\0';
            delete_contact(name);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            char *name = p;
            char *space = strchr(name, ' ');
            if (space) *space = '\0';
            find_contact_print(name);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_contacts();
        }
    }
    free_all();
    return 0;
}