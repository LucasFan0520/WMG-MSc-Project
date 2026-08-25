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
    for (Contact *p = head; p; p = p->next) {
        if (strcmp(p->name, name) == 0) {
            return p;
        }
    }
    return NULL;
}

void add_contact(const char *name, const char *email, const char *phone) {
    Contact *p = malloc(sizeof(Contact));
    p->name = strdup(name);
    p->email = strdup(email);
    p->phone = strdup(phone);
    p->note = strdup("");
    p->next = head;
    head = p;
}

void note_contact(const char *name, const char *text) {
    Contact *p = find_contact(name);
    if (p) {
        free(p->note);
        p->note = strdup(text);
    }
}

void merge_contact(const char *keep, const char *remove) {
    Contact *k = find_contact(keep);
    Contact *r = find_contact(remove);
    if (k && r) {
        char *buf = malloc(strlen(k->note) + strlen(r->note) + 2);
        sprintf(buf, "%s %s", k->note, r->note);
        free(k->note);
        k->note = buf;
        Contact *p = head;
        Contact *prev = NULL;
        while (p) {
            if (p == r) {
                Contact *next = p->next;
                free(p->name);
                free(p->email);
                free(p->phone);
                free(p->note);
                free(p);
                if (prev) {
                    prev->next = next;
                } else {
                    head = next;
                }
                return;
            }
            prev = p;
            p = p->next;
        }
    }
}

void delete_contact(const char *name) {
    Contact *p = head;
    Contact *prev = NULL;
    while (p) {
        if (strcmp(p->name, name) == 0) {
            Contact *next = p->next;
            free(p->name);
            free(p->email);
            free(p->phone);
            free(p->note);
            free(p);
            if (prev) {
                prev->next = next;
            } else {
                head = next;
            }
            return;
        }
        prev = p;
        p = p->next;
    }
}

void find_contact_print(const char *name) {
    Contact *p = find_contact(name);
    if (p) {
        printf("%s %s %s %s\n", p->name, p->email, p->phone, p->note);
    }
}

void list_contacts() {
    for (Contact *p = head; p; p = p->next) {
        printf("%s %s %s %s\n", p->name, p->email, p->phone, p->note);
    }
}

int main() {
    char line[10000];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        char cmd[20];
        char a[1000];
        char b[1000];
        char c[1000];
        char d[6000];
        b[0] = 0;
        c[0] = 0;
        d[0] = 0;
        sscanf(line, "%s %s %s %s %[^\n]", cmd, a, b, c, d);
        if (strcmp(cmd, "ADD") == 0) {
            add_contact(a, b, c);
        } else if (strcmp(cmd, "NOTE") == 0) {
            char text[9000];
            sscanf(line, "%*s %*s %[^\n]", text);
            note_contact(a, text);
        } else if (strcmp(cmd, "MERGE") == 0) {
            merge_contact(a, b);
        } else if (strcmp(cmd, "DELETE") == 0) {
            delete_contact(a);
        } else if (strcmp(cmd, "FIND") == 0) {
            find_contact_print(a);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_contacts();
        }
    }
    while (head) {
        Contact *n = head->next;
        free(head->name);
        free(head->email);
        free(head->phone);
        free(head->note);
        free(head);
        head = n;
    }
    return 0;
}