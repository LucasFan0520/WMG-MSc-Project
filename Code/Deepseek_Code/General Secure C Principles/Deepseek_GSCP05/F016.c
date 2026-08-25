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
    c->next = contacts;
    contacts = c;
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
    printf("%s %s %s\n", c->name, c->email, c->phone);
}

void report_contacts(void) {
    Contact *c = contacts;
    while (c) {
        printf("%s %s %s\n", c->name, c->email, c->phone);
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
        free(c);
        c = next;
    }
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Missing file argument\n");
        return 1;
    }
    FILE *f = fopen(argv[1], "r");
    if (!f) {
        perror("fopen");
        return 1;
    }
    char line[4096];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = '\0';
        // parse name,email,phone
        char *name = strtok(line, ",");
        if (!name) continue;
        char *email = strtok(NULL, ",");
        if (!email) continue;
        char *phone = strtok(NULL, ",");
        if (!phone) continue;
        add_contact(name, email, phone);
    }
    fclose(f);

    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) != 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *name = strtok(p, ",");
            if (!name) continue;
            char *email = strtok(NULL, ",");
            if (!email) continue;
            char *phone = strtok(NULL, ",");
            if (!phone) continue;
            add_contact(name, email, phone);
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
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_contacts();
        }
    }
    free_all();
    return 0;
}