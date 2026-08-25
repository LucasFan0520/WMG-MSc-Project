/* F015.c */
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

    while (getline(&line, &len, stdin) != -1) {
        size_t str_len = strlen(line);
        while (str_len > 0 && (line[str_len - 1] == '\n' || line[str_len - 1] == '\r')) {
            line[str_len - 1] = '\0';
            str_len--;
        }
        char *cmd = line;
        while (*cmd == ' ') cmd++;
        if (*cmd == '\0') continue;
        char *p = cmd;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            break;
        }
        while (*p == ' ') p++;

        if (strcmp(cmd, "ADD") == 0) {
            char *name_str = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *email_str = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *phone_str = p;

            if (count >= capacity) {
                size_t new_cap = capacity == 0 ? 4 : capacity * 2;
                Contact *temp = realloc(contacts, new_cap * sizeof(Contact));
                if (!temp) break;
                contacts = temp;
                capacity = new_cap;
            }
            contacts[count].name = strdup(name_str);
            contacts[count].email = strdup(email_str);
            contacts[count].phone = strdup(phone_str);
            contacts[count].note = strdup("");
            if (contacts[count].name && contacts[count].email && contacts[count].phone && contacts[count].note) {
                count++;
            }
        } else if (strcmp(cmd, "NOTE") == 0) {
            char *name_str = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *text_str = p;

            for (size_t i = 0; i < count; i++) {
                if (strcmp(contacts[i].name, name_str) == 0) {
                    char *temp = strdup(text_str);
                    if (temp) {
                        free(contacts[i].note);
                        contacts[i].note = temp;
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "MERGE") == 0) {
            char *keep_str = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *remove_str = p;

            Contact *keep_c = NULL;
            Contact *remove_c = NULL;
            size_t remove_idx = 0;

            for (size_t i = 0; i < count; i++) {
                if (strcmp(contacts[i].name, keep_str) == 0) {
                    keep_c = &contacts[i];
                }
                if (strcmp(contacts[i].name, remove_str) == 0) {
                    remove_c = &contacts[i];
                    remove_idx = i;
                }
            }

            if (keep_c && remove_c) {
                size_t len1 = strlen(keep_c->note);
                size_t len2 = strlen(remove_c->note);
                char *new_note = malloc(len1 + len2 + 2);
                if (new_note) {
                    strcpy(new_note, keep_c->note);
                    if (len1 > 0 && len2 > 0) {
                        strcat(new_note, " ");
                    }
                    strcat(new_note, remove_c->note);
                    free(keep_c->note);
                    keep_c->note = new_note;
                }
                free(remove_c->name);
                free(remove_c->email);
                free(remove_c->phone);
                free(remove_c->note);
                for (size_t j = remove_idx; j < count - 1; j++) {
                    contacts[j] = contacts[j + 1];
                }
                count--;
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name_str = p;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(contacts[i].name, name_str) == 0) {
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
            char *name_str = p;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(contacts[i].name, name_str) == 0) {
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
    free(line);
    for (size_t i = 0; i < count; i++) {
        free(contacts[i].name);
        free(contacts[i].email);
        free(contacts[i].phone);
        free(contacts[i].note);
    }
    free(contacts);
    return 0;
}
