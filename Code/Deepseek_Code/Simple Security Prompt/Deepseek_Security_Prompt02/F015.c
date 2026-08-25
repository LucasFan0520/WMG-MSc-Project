/* F015.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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

void free_contact(Contact *c) {
    free(c->name);
    free(c->email);
    free(c->phone);
    free(c->note);
    free(c);
}

void add_contact(const char *name, const char *email, const char *phone) {
    Contact *c = find_contact(name);
    if (c) {
        free(c->email); free(c->phone);
        c->email = strdup(email);
        c->phone = strdup(phone);
        return;
    }
    c = malloc(sizeof(Contact));
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
    size_t newlen = strlen(k->note) + strlen(r->note) + 2;
    char *new_note = malloc(newlen);
    strcpy(new_note, k->note);
    strcat(new_note, " ");
    strcat(new_note, r->note);
    free(k->note);
    k->note = new_note;
    delete_contact(remove);
}

void delete_contact(const char *name) {
    Contact *prev = NULL, *curr = contacts;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else contacts = curr->next;
            free_contact(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void find_contact_print(const char *name) {
    Contact *c = find_contact(name);
    if (!c) {
        printf("NOT_FOUND\n");
        return;
    }
    printf("%s %s %s %s\n", c->name, c->email, c->phone, c->note);
}

void list_contacts(void) {
    Contact *c = contacts;
    while (c) {
        printf("%s %s %s %s\n", c->name, c->email, c->phone, c->note);
        c = c->next;
    }
}

void trim_newline(char *s) {
    size_t len = strlen(s);
    if (len > 0 && s[len-1] == '\n') s[len-1] = '\0';
}

int main(void) {
    char line[4096];
    char cmd[32];
    char name[256], email[256], phone[256], text[4096];

    while (fgets(line, sizeof(line), stdin)) {
        trim_newline(line);
        if (strcmp(line, "END") == 0) break;
        if (sscanf(line, "%31s", cmd) != 1) continue;

        if (strcmp(cmd, "ADD") == 0) {
            char *p = line + 3;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s %255s %255s", name, email, phone) != 3) continue;
            add_contact(name, email, phone);
        } else if (strcmp(cmd, "NOTE") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", name) != 1) continue;
            p += strlen(name);
            while (*p == ' ') p++;
            note_contact(name, p);
        } else if (strcmp(cmd, "MERGE") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s %255s", name, text) != 2) continue; 
            merge_contacts(name, text);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", name) == 1) delete_contact(name);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", name) == 1) find_contact_print(name);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_contacts();
        }
    }

    while (contacts) {
        Contact *tmp = contacts;
        contacts = contacts->next;
        free_contact(tmp);
    }
    return 0;
}