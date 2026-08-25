// F016.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Contact {
    char *name;
    char *email;
    char *phone;
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
    c->next = NULL;
    if (tail) tail->next = c;
    else head = c;
    tail = c;
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
    printf("%s %s %s\n", c->name, c->email, c->phone);
}

void report_contacts(void) {
    Contact *c = head;
    while (c) {
        printf("%s %s %s\n", c->name, c->email, c->phone);
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
        free(c);
        c = next;
    }
}

int main(int argc, char **argv) {
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char line[1024];
            while (fgets(line, sizeof(line), f)) {
                line[strcspn(line, "\n")] = 0;
                char *name = strtok(line, ",");
                char *email = strtok(NULL, ",");
                char *phone = strtok(NULL, ",");
                if (name && email && phone) {
                    add_contact(name, email, phone);
                }
            }
            fclose(f);
        }
    }
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        char *rest = strtok(NULL, "");
        if (!rest) rest = "";
        if (strcmp(cmd, "ADD") == 0) {
            char *name = strtok(rest, ",");
            char *email = strtok(NULL, ",");
            char *phone = strtok(NULL, ",");
            if (name && email && phone) add_contact(name, email, phone);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = strtok(rest, " ");
            if (name) delete_contact(name);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *name = strtok(rest, " ");
            if (name) find_contact_print(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_contacts();
        }
    }
    free_all();
    return 0;
}