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

Contact *find_contact(char *name) {
    for (Contact *c = head; c; c = c->next) {
        if (strcmp(c->name, name) == 0) return c;
    }
    return NULL;
}

void add_contact(char *name, char *email, char *phone) {
    if (find_contact(name)) return;
    Contact *c = malloc(sizeof(Contact));
    c->name = strdup(name);
    c->email = strdup(email);
    c->phone = strdup(phone);
    c->next = head;
    head = c;
}

void delete_contact(char *name) {
    Contact *prev = NULL;
    Contact *curr = head;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            free(curr->name);
            free(curr->email);
            free(curr->phone);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void find_contact_info(char *name) {
    Contact *c = find_contact(name);
    if (c) printf("%s,%s,%s\n", c->name, c->email, c->phone);
}

void report_contacts() {
    for (Contact *c = head; c; c = c->next) {
        printf("%s,%s,%s\n", c->name, c->email, c->phone);
    }
}

void parse_csv_line(char *line) {
    char *name = strtok(line, ",");
    char *email = strtok(NULL, ",");
    char *phone = strtok(NULL, "\n");
    if (name && email && phone) {
        add_contact(name, email, phone);
    }
}

int main(int argc, char *argv[]) {
    if (argc > 1) {
        FILE *file = fopen(argv[1], "r");
        if (file) {
            char line[1000];
            while (fgets(line, sizeof(line), file)) {
                parse_csv_line(line);
            }
            fclose(file);
        }
    }
    char cmd[20], line[1000], name[100];
    while (scanf("%19s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "ADD") == 0) {
            fgets(line, sizeof(line), stdin);
            line[strcspn(line, "\n")] = 0;
            parse_csv_line(line);
        } else if (strcmp(cmd, "DELETE") == 0) {
            scanf("%99s", name);
            delete_contact(name);
        } else if (strcmp(cmd, "FIND") == 0) {
            scanf("%99s", name);
            find_contact_info(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_contacts();
        }
    }
    return 0;
}