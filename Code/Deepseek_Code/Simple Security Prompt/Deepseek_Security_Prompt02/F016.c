/* F016.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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

void free_contact(Contact *c) {
    free(c->name);
    free(c->email);
    free(c->phone);
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
    c->next = contacts;
    contacts = c;
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
    printf("%s,%s,%s\n", c->name, c->email, c->phone);
}

void report_contacts(void) {
    Contact *c = contacts;
    while (c) {
        printf("%s,%s,%s\n", c->name, c->email, c->phone);
        c = c->next;
    }
}

void load_file(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) return;
    char line[4096];
    while (fgets(line, sizeof(line), f)) {
        char *nl = strchr(line, '\n');
        if (nl) *nl = '\0';
        char name[256], email[256], phone[256];
        if (sscanf(line, "%255[^,],%255[^,],%255[^,]", name, email, phone) == 3) {
            add_contact(name, email, phone);
        }
    }
    fclose(f);
}

void trim_newline(char *s) {
    size_t len = strlen(s);
    if (len > 0 && s[len-1] == '\n') s[len-1] = '\0';
}

int main(int argc, char *argv[]) {
    if (argc > 1) load_file(argv[1]);

    char line[4096];
    char cmd[32];
    char name[256], email[256], phone[256];

    while (fgets(line, sizeof(line), stdin)) {
        trim_newline(line);
        if (strcmp(line, "END") == 0) break;
        if (sscanf(line, "%31s", cmd) != 1) continue;

        if (strcmp(cmd, "ADD") == 0) {
            char *p = line + 3;
            while (*p == ' ') p++;
            if (sscanf(p, "%255[^,],%255[^,],%255[^,]", name, email, phone) != 3) continue;
            add_contact(name, email, phone);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", name) == 1) delete_contact(name);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", name) == 1) find_contact_print(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_contacts();
        }
    }

    while (contacts) {
        Contact *tmp = contacts;
        contacts = contacts->next;
        free_contact(tmp);
    }
    return 0;
}