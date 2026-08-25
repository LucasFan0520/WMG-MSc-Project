/* F016.c */
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

void add_contact(char *name, char *email, char *phone) {
    Contact *new = malloc(sizeof(Contact));
    new->name = strdup(name);
    new->email = strdup(email);
    new->phone = strdup(phone);
    new->next = head;
    head = new;
}

Contact *find_contact(char *name) {
    for (Contact *c = head; c; c = c->next)
        if (strcmp(c->name, name) == 0) return c;
    return NULL;
}

void delete_contact(char *name) {
    Contact *prev = NULL;
    for (Contact *c = head; c; c = c->next) {
        if (strcmp(c->name, name) == 0) {
            if (prev) prev->next = c->next;
            else head = c->next;
            free(c->name);
            free(c->email);
            free(c->phone);
            free(c);
            return;
        }
        prev = c;
    }
}

void find_contact_print(char *name) {
    Contact *c = find_contact(name);
    if (c) printf("%s,%s,%s\n", c->name, c->email, c->phone);
}

void report_all(void) {
    for (Contact *c = head; c; c = c->next)
        printf("%s,%s,%s\n", c->name, c->email, c->phone);
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

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <inputfile>\n", argv[0]);
        return 1;
    }
    FILE *f = fopen(argv[1], "r");
    if (!f) {
        perror("fopen");
        return 1;
    }
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, f) != -1) {
        line[strcspn(line, "\n")] = '\0';
        char *name = strtok(line, ",");
        char *email = strtok(NULL, ",");
        char *phone = strtok(NULL, ",");
        if (name && email && phone) {
            add_contact(name, email, phone);
        }
    }
    free(line);
    fclose(f);

    // Now read commands from stdin
    line = NULL;
    len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], arg1[256], arg2[256], arg3[256];
        int n = sscanf(line, "%15s %255s %255s %255s", cmd, arg1, arg2, arg3);
        if (n < 1) continue;
        if (strcmp(cmd, "ADD") == 0 && n == 4) {
            add_contact(arg1, arg2, arg3);
        } else if (strcmp(cmd, "DELETE") == 0 && n == 2) {
            delete_contact(arg1);
        } else if (strcmp(cmd, "FIND") == 0 && n == 2) {
            find_contact_print(arg1);
        } else if (strcmp(cmd, "REPORT") == 0 && n == 1) {
            report_all();
        }
    }
    free(line);
    free_all();
    return 0;
}