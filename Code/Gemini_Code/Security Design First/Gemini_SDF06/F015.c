// F015.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *email;
    char *phone;
    char *note;
} Contact;

int main(void) {
    Contact *contacts = NULL;
    size_t count = 0;
    size_t capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0') continue;
        char *cmd = p;
        while (*p && *p != ' ' && *p != '\t') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            break;
        }
        if (strcmp(cmd, "ADD") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *email = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *phone = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';

            if (*name == '\0') continue;

            if (count >= capacity) {
                size_t new_cap = capacity == 0 ? 4 : capacity * 2;
                Contact *new_contacts = realloc(contacts, new_cap * sizeof(Contact));
                if (!new_contacts) continue;
                contacts = new_contacts;
                capacity = new_cap;
            }
            char *n = strdup(name);
            char *e = strdup(email);
            char *ph = strdup(phone);
            char *nt = strdup("");
            if (n && e && ph && nt) {
                contacts[count].name = n;
                contacts[count].email = e;
                contacts[count].phone = ph;
                contacts[count].note = nt;
                count++;
            } else {
                free(n);
                free(e);
                free(ph);
                free(nt);
            }
        } else if (strcmp(cmd, "NOTE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *text = p;

            if (*name == '\0') continue;

            for (size_t i = 0; i < count; i++) {
                if (strcmp(contacts[i].name, name) == 0) {
                    char *t = strdup(text);
                    if (t) {
                        free(contacts[i].note);
                        contacts[i].note = t;
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "MERGE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *keep = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *remove = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';

            if (*keep == '\0' || *remove == '\0') continue;

            int idx_keep = -1, idx_remove = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(contacts[i].name, keep) == 0) idx_keep = (int)i;
                if (strcmp(contacts[i].name, remove) == 0) idx_remove = (int)i;
            }

            if (idx_keep != -1 && idx_remove != -1) {
                size_t len1 = strlen(contacts[idx_keep].note);
                size_t len2 = strlen(contacts[idx_remove].note);
                char *new_note = malloc(len1 + len2 + 2);
                if (new_note) {
                    strcpy(new_note, contacts[idx_keep].note);
                    if (len1 > 0 && len2 > 0) {
                        strcat(new_note, " ");
                    }
                    strcat(new_note, contacts[idx_remove].note);
                    free(contacts[idx_keep].note);
                    contacts[idx_keep].note = new_note;

                    free(contacts[idx_remove].name);
                    free(contacts[idx_remove].email);
                    free(contacts[idx_remove].phone);
                    free(contacts[idx_remove].note);
                    for (size_t j = (size_t)idx_remove; j < count - 1; j++) {
                        contacts[j] = contacts[j + 1];
                    }
                    count--;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            if (*name == '\0') continue;

            for (size_t i = 0; i < count; i++) {
                if (strcmp(contacts[i].name, name) == 0) {
                    free(contacts[i].name);
                    free(contacts[i].email);
                    free(contacts[i].phone);
                    free(contacts[i].note);
                    for (size_t j = i; j < count - 1; j++) {
                        contacts[j] = contacts[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            if (*name == '\0') continue;

            for (size_t i = 0; i < count; i++) {
                if (strcmp(contacts[i].name, name) == 0) {
                    printf("%s %s %s %s\n", contacts[i].name, contacts[i].email, contacts[i].phone, contacts[i].note);
                    break;
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s %s %s\n", contacts[i].name, contacts[i].email, contacts[i].phone, contacts[i].note);
            }
        }
    }

    for (size_t i = 0; i < count; i++) {
        free(contacts[i].name);
        free(contacts[i].email);
        free(contacts[i].phone);
        free(contacts[i].note);
    }
    free(contacts);
    free(line);
    return 0;
}
