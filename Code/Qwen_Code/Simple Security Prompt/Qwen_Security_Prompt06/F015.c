// F015.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line() {
    size_t cap = 256, len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    while (1) {
        if (!fgets(buf + len, cap - len, stdin)) {
            if (len == 0) { free(buf); return NULL; }
            break;
        }
        len = strlen(buf);
        if (len > 0 && buf[len-1] == '\n') { buf[len-1] = '\0'; break; }
        if (len == cap - 1) {
            cap *= 2;
            char *tmp = realloc(buf, cap);
            if (!tmp) { free(buf); return NULL; }
            buf = tmp;
        }
    }
    return buf;
}

typedef struct Contact {
    char *name;
    char *email;
    char *phone;
    char *note;
    struct Contact *next;
} Contact;

Contact *contacts = NULL;

Contact *find_contact(const char *name) {
    for (Contact *c = contacts; c; c = c->next) {
        if (strcmp(c->name, name) == 0) return c;
    }
    return NULL;
}

void add_contact(const char *name, const char *email, const char *phone) {
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
    if (c) {
        free(c->note);
        c->note = strdup(text);
    }
}

void merge_contacts(const char *keep, const char *remove) {
    Contact *k = find_contact(keep);
    Contact *r = find_contact(remove);
    if (k && r) {
        size_t len = strlen(k->note) + strlen(r->note) + 2;
        char *new_note = malloc(len);
        snprintf(new_note, len, "%s %s", k->note, r->note);
        free(k->note);
        k->note = new_note;
        
        Contact *prev = NULL, *curr = contacts;
        while (curr) {
            if (curr == r) {
                if (prev) prev->next = curr->next;
                else contacts = curr->next;
                free(curr->name);
                free(curr->email);
                free(curr->phone);
                free(curr->note);
                free(curr);
                return;
            }
            prev = curr;
            curr = curr->next;
        }
    }
}

void delete_contact(const char *name) {
    Contact *prev = NULL, *curr = contacts;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else contacts = curr->next;
            free(curr->name);
            free(curr->email);
            free(curr->phone);
            free(curr->note);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void find_print_contact(const char *name) {
    Contact *c = find_contact(name);
    if (c) {
        printf("%s %s %s %s\n", c->name, c->email, c->phone, c->note);
    }
}

void list_contacts() {
    for (Contact *c = contacts; c; c = c->next) {
        printf("%s %s %s %s\n", c->name, c->email, c->phone, c->note);
    }
}

int main() {
    char *line;
    while ((line = read_line())) {
        if (strcmp(line, "END") == 0) break;
        char cmd[16] = {0}, arg1[256] = {0}, arg2[256] = {0}, arg3[256] = {0};
        sscanf(line, "%15s %255s %255s %255s", cmd, arg1, arg2, arg3);
        char *text = strchr(line, ' ');
        if (text) {
            text++;
            while (*text == ' ') text++;
            text = strchr(text, ' ');
            if (text) {
                text++;
                while (*text == ' ') text++;
            } else {
                text = NULL;
            }
        }
        
        if (strcmp(cmd, "ADD") == 0) {
            add_contact(arg1, arg2, arg3);
        } else if (strcmp(cmd, "NOTE") == 0 && text) {
            note_contact(arg1, text);
        } else if (strcmp(cmd, "MERGE") == 0) {
            merge_contacts(arg1, arg2);
        } else if (strcmp(cmd, "DELETE") == 0) {
            delete_contact(arg1);
        } else if (strcmp(cmd, "FIND") == 0) {
            find_print_contact(arg1);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_contacts();
        }
        free(line);
    }
    
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
    return 0;
}