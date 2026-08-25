// F016.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Contact {
    char *name, *email, *phone;
    struct Contact *next;
} Contact;

Contact *head = NULL;

void load_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    char line[1024];
    while (fgets(line, sizeof(line), f)) {
        size_t l = strlen(line);
        if (l > 0 && line[l-1] == '\n') line[l-1] = '\0';
        char *n = strtok(line, ",");
        char *e = strtok(NULL, ",");
        char *p = strtok(NULL, ",");
        if (n && e && p) {
            Contact *c = malloc(sizeof(Contact));
            c->name = strdup(n); c->email = strdup(e); c->phone = strdup(p); c->next = head; head = c;
        }
    }
    fclose(f);
}

int main(int argc, char *argv[]) {
    if (argc > 1) load_file(argv[1]);
    char cmd[20], text[1024];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "ADD") == 0) {
            scanf("%s", text);
            char *n = strtok(text, ",");
            char *e = strtok(NULL, ",");
            char *p = strtok(NULL, ",");
            if (n && e && p) {
                Contact *c = malloc(sizeof(Contact));
                c->name = strdup(n); c->email = strdup(e); c->phone = strdup(p); c->next = head; head = c;
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            scanf("%s", text);
            Contact *prev = NULL, *curr = head;
            while (curr) {
                if (strcmp(curr->name, text) == 0) {
                    if (prev) prev->next = curr->next; else head = curr->next;
                    free(curr->name); free(curr->email); free(curr->phone); free(curr); break;
                }
                prev = curr; curr = curr->next;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            scanf("%s", text);
            for (Contact *c = head; c; c = c->next)
                if (strcmp(c->name, text) == 0) { printf("%s,%s,%s\n", c->name, c->email, c->phone); break; }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (Contact *c = head; c; c = c->next) printf("%s,%s,%s\n", c->name, c->email, c->phone);
        }
    }
    return 0;
}