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
Contact *tail = NULL;

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
    c->name = malloc(strlen(name)+1);
    strcpy(c->name, name);
    c->email = malloc(strlen(email)+1);
    strcpy(c->email, email);
    c->phone = malloc(strlen(phone)+1);
    strcpy(c->phone, phone);
    c->note = malloc(1);
    c->note[0] = 0;
    c->next = NULL;
    if (tail) tail->next = c;
    else head = c;
    tail = c;
}

void note_contact(const char *name, const char *note) {
    Contact *c = find_contact(name);
    if (!c) return;
    free(c->note);
    c->note = malloc(strlen(note)+1);
    strcpy(c->note, note);
}

void merge_contacts(const char *keep, const char *remove) {
    Contact *ck = find_contact(keep);
    Contact *cr = find_contact(remove);
    if (!ck || !cr) return;
    // combine notes
    size_t newlen = strlen(ck->note) + strlen(cr->note) + 2;
    char *new_note = malloc(newlen);
    strcpy(new_note, ck->note);
    if (strlen(ck->note) > 0 && strlen(cr->note) > 0) strcat(new_note, " ");
    strcat(new_note, cr->note);
    free(ck->note);
    ck->note = new_note;
    // delete removed
    Contact *prev = NULL;
    Contact *c = head;
    while (c) {
        if (c == cr) {
            if (prev) prev->next = c->next;
            else head = c->next;
            if (c == tail) tail = prev;
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
            if (c == tail) tail = prev;
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
    printf("%s %s %s %s\n", c->name, c->email, c->phone, c->note);
}

void list_contacts(void) {
    Contact *c = head;
    while (c) {
        printf("%s %s %s %s\n", c->name, c->email, c->phone, c->note);
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

int main(void) {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        char *rest = strtok(NULL, "");
        if (!rest) rest = "";
        if (strcmp(cmd, "ADD") == 0) {
            char *name = strtok(rest, " ");
            if (!name) continue;
            char *email = strtok(NULL, " ");
            if (!email) continue;
            char *phone = strtok(NULL, " ");
            if (!phone) phone = "";
            add_contact(name, email, phone);
        } else if (strcmp(cmd, "NOTE") == 0) {
            char *name = strtok(rest, " ");
            if (!name) continue;
            char *text = strtok(NULL, "");
            if (!text) text = "";
            note_contact(name, text);
        } else if (strcmp(cmd, "MERGE") == 0) {
            char *keep = strtok(rest, " ");
            char *remove = strtok(NULL, " ");
            if (keep && remove) merge_contacts(keep, remove);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = strtok(rest, " ");
            if (name) delete_contact(name);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *name = strtok(rest, " ");
            if (name) find_contact_print(name);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_contacts();
        }
    }
    free_all();
    return 0;
}