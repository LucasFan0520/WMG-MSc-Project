/* F016.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct contact {
    char *name;
    char *email;
    char *phone;
    struct contact *next;
} contact_t;

contact_t *head = NULL;
contact_t *tail = NULL;

contact_t *find_contact(const char *name) {
    contact_t *c = head;
    while (c) {
        if (strcmp(c->name, name) == 0) return c;
        c = c->next;
    }
    return NULL;
}

void add_contact(const char *name, const char *email, const char *phone) {
    if (find_contact(name)) return;
    contact_t *c = malloc(sizeof(contact_t));
    if (!c) return;
    c->name = strdup(name);
    c->email = strdup(email);
    c->phone = strdup(phone);
    c->next = NULL;
    if (!c->name || !c->email || !c->phone) {
        free(c->name); free(c->email); free(c->phone); free(c);
        return;
    }
    if (tail) tail->next = c;
    else head = c;
    tail = c;
}

void delete_contact(const char *name) {
    contact_t *prev = NULL, *c = head;
    while (c) {
        if (strcmp(c->name, name) == 0) {
            if (prev) prev->next = c->next;
            else head = c->next;
            if (c == tail) tail = prev;
            free(c->name);
            free(c->email);
            free(c->phone);
            free(c);
            return;
        }
        prev = c;
        c = c->next;
    }
}

void find_contact_print(const char *name) {
    contact_t *c = find_contact(name);
    if (!c) {
        printf("NOT_FOUND\n");
        return;
    }
    printf("%s,%s,%s\n", c->name, c->email, c->phone);
}

void report_contacts(void) {
    contact_t *c = head;
    while (c) {
        printf("%s,%s,%s\n", c->name, c->email, c->phone);
        c = c->next;
    }
}

void free_all(void) {
    contact_t *c = head;
    while (c) {
        contact_t *nxt = c->next;
        free(c->name);
        free(c->email);
        free(c->phone);
        free(c);
        c = nxt;
    }
}

int main(int argc, char **argv) {
    if (argc < 2) return 1;
    FILE *f = fopen(argv[1], "r");
    if (f) {
        char *line = NULL;
        size_t len = 0;
        while (getline(&line, &len, f) != -1) {
            line[strcspn(line, "\n")] = 0;
            char name[256], email[256], phone[256];
            if (sscanf(line, "%[^,],%[^,],%[^,]", name, email, phone) == 3) {
                add_contact(name, email, phone);
            }
        }
        free(line);
        fclose(f);
    }
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[10], a[256], b[256], c[256];
        if (sscanf(line, "%9s %255s", cmd, a) >= 1) {
            if (strcmp(cmd, "ADD") == 0) {
                if (sscanf(line, "%*s %[^,],%[^,],%[^,]", a, b, c) == 3)
                    add_contact(a, b, c);
            } else if (strcmp(cmd, "DELETE") == 0) {
                delete_contact(a);
            } else if (strcmp(cmd, "FIND") == 0) {
                find_contact_print(a);
            } else if (strcmp(cmd, "REPORT") == 0) {
                report_contacts();
            }
        }
    }
    free(line);
    free_all();
    return 0;
}