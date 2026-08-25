// F016.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 4096

typedef struct Contact {
    char *name;
    char *email;
    char *phone;
    struct Contact *next;
} Contact;

Contact *contacts = NULL;

Contact* find_contact(const char *name) {
    for (Contact *c = contacts; c; c = c->next)
        if (strcmp(c->name, name) == 0) return c;
    return NULL;
}

void load_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = 0;
        char *name = strtok(line, ",");
        char *email = strtok(NULL, ",");
        char *phone = strtok(NULL, ",");
        if (name && email && phone) {
            Contact *c = malloc(sizeof(Contact));
            if (!c) continue;
            c->name = strdup(name);
            c->email = strdup(email);
            c->phone = strdup(phone);
            c->next = contacts;
            contacts = c;
        }
    }
    fclose(f);
}

int main(int argc, char *argv[]) {
    if (argc > 1) load_file(argv[1]);
    
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        
        if (strcmp(cmd, "ADD") == 0) {
            char name[256], email[256], phone[256];
            if (sscanf(rest, "%255[^,],%255[^,],%255s", name, email, phone) != 3) continue;
            if (find_contact(name)) continue;
            Contact *c = malloc(sizeof(Contact));
            if (!c) continue;
            c->name = strdup(name);
            c->email = strdup(email);
            c->phone = strdup(phone);
            c->next = contacts;
            contacts = c;
        } else if (strcmp(cmd, "DELETE") == 0) {
            char name[256];
            if (sscanf(rest, "%255s", name) != 1) continue;
            Contact *prev = NULL;
            for (Contact *c = contacts; c; prev = c, c = c->next) {
                if (strcmp(c->name, name) == 0) {
                    if (prev) prev->next = c->next;
                    else contacts = c->next;
                    free(c->name);
                    free(c->email);
                    free(c->phone);
                    free(c);
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char name[256];
            if (sscanf(rest, "%255s", name) != 1) continue;
            Contact *c = find_contact(name);
            if (c) printf("%s,%s,%s\n", c->name, c->email, c->phone);
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (Contact *c = contacts; c; c = c->next)
                printf("%s,%s,%s\n", c->name, c->email, c->phone);
        }
    }
    while (contacts) {
        Contact *c = contacts;
        contacts = contacts->next;
        free(c->name);
        free(c->email);
        free(c->phone);
        free(c);
    }
    return 0;
}