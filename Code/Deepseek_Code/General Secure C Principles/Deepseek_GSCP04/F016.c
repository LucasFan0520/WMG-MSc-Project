/* F016.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Contact {
    char *name;
    char *email;
    char *phone;
} Contact;

static Contact *contacts = NULL;
static size_t ccount = 0, ccap = 0;

static Contact *find_contact(const char *name) {
    for (size_t i = 0; i < ccount; i++) {
        if (strcmp(contacts[i].name, name) == 0)
            return &contacts[i];
    }
    return NULL;
}

static void add_contact(const char *name, const char *email, const char *phone) {
    if (ccount == ccap) {
        ccap = ccap ? ccap * 2 : 4;
        contacts = realloc(contacts, ccap * sizeof(Contact));
        if (!contacts) { perror("realloc"); exit(1); }
    }
    contacts[ccount].name = strdup(name);
    contacts[ccount].email = strdup(email);
    contacts[ccount].phone = strdup(phone);
    ccount++;
}

static void delete_contact(const char *name) {
    for (size_t i = 0; i < ccount; i++) {
        if (strcmp(contacts[i].name, name) == 0) {
            free(contacts[i].name);
            free(contacts[i].email);
            free(contacts[i].phone);
            for (size_t j = i; j < ccount - 1; j++)
                contacts[j] = contacts[j + 1];
            ccount--;
            return;
        }
    }
}

static void find_contact_print(const char *name) {
    Contact *c = find_contact(name);
    if (!c) return;
    printf("%s,%s,%s\n", c->name, c->email, c->phone);
}

static void report_contacts(void) {
    for (size_t i = 0; i < ccount; i++) {
        printf("%s,%s,%s\n", contacts[i].name, contacts[i].email, contacts[i].phone);
    }
}

static void load_file(const char *fname) {
    FILE *f = fopen(fname, "r");
    if (!f) return;
    char line[4096];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = '\0';
        char *name = strtok(line, ",");
        char *email = strtok(NULL, ",");
        char *phone = strtok(NULL, ",");
        if (name && email && phone) {
            // skip if duplicate? We'll just add (maybe update if exists? spec doesn't say)
            add_contact(name, email, phone);
        }
    }
    fclose(f);
}

int main(int argc, char **argv) {
    if (argc > 1) load_file(argv[1]);
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *csv = strtok(NULL, "");
            if (!csv) continue;
            char *name = strtok(csv, ",");
            char *email = strtok(NULL, ",");
            char *phone = strtok(NULL, ",");
            if (name && email && phone) {
                if (find_contact(name)) {
                    // update
                    Contact *c = find_contact(name);
                    free(c->email); c->email = strdup(email);
                    free(c->phone); c->phone = strdup(phone);
                } else {
                    add_contact(name, email, phone);
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = strtok(NULL, " ");
            if (name) delete_contact(name);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *name = strtok(NULL, " ");
            if (name) find_contact_print(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_contacts();
        }
    }
    for (size_t i = 0; i < ccount; i++) {
        free(contacts[i].name);
        free(contacts[i].email);
        free(contacts[i].phone);
    }
    free(contacts);
    return 0;
}