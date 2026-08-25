/* F016.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *email;
    char *phone;
} Contact;

int main(int argc, char *argv[]) {
    if (argc < 2) return 1;

    Contact *contacts = NULL;
    size_t count = 0;
    size_t capacity = 0;

    FILE *fp = fopen(argv[1], "r");
    char *line = NULL;
    size_t len = 0;

    if (fp) {
        while (getline(&line, &len, fp) != -1) {
            size_t str_len = strlen(line);
            while (str_len > 0 && (line[str_len - 1] == '\n' || line[str_len - 1] == '\r')) {
                line[str_len - 1] = '\0';
                str_len--;
            }
            char *c1 = strchr(line, ',');
            if (c1) {
                char *c2 = strchr(c1 + 1, ',');
                if (c2) {
                    *c1 = '\0';
                    *c2 = '\0';
                    char *name = line;
                    char *email = c1 + 1;
                    char *phone = c2 + 1;

                    if (count >= capacity) {
                        size_t new_cap = capacity == 0 ? 4 : capacity * 2;
                        Contact *temp = realloc(contacts, new_cap * sizeof(Contact));
                        if (!temp) break;
                        contacts = temp;
                        capacity = new_cap;
                    }
                    contacts[count].name = strdup(name);
                    contacts[count].email = strdup(email);
                    contacts[count].phone = strdup(phone);
                    if (contacts[count].name && contacts[count].email && contacts[count].phone) {
                        count++;
                    }
                }
            }
        }
        fclose(fp);
    }

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
            char *c1 = strchr(p, ',');
            if (c1) {
                char *c2 = strchr(c1 + 1, ',');
                if (c2) {
                    *c1 = '\0';
                    *c2 = '\0';
                    char *name = p;
                    char *email = c1 + 1;
                    char *phone = c2 + 1;

                    if (count >= capacity) {
                        size_t new_cap = capacity == 0 ? 4 : capacity * 2;
                        Contact *temp = realloc(contacts, new_cap * sizeof(Contact));
                        if (!temp) break;
                        contacts = temp;
                        capacity = new_cap;
                    }
                    contacts[count].name = strdup(name);
                    contacts[count].email = strdup(email);
                    contacts[count].phone = strdup(phone);
                    if (contacts[count].name && contacts[count].email && contacts[count].phone) {
                        count++;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = p;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(contacts[i].name, name) == 0) {
                    free(contacts[i].name);
                    free(contacts[i].email);
                    free(contacts[i].phone);
                    for (size_t j = i; j < count - 1; j++) {
                        contacts[j] = contacts[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *name = p;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(contacts[i].name, name) == 0) {
                    printf("%s,%s,%s\n", contacts[i].name, contacts[i].email, contacts[i].phone);
                    break;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s,%s,%s\n", contacts[i].name, contacts[i].email, contacts[i].phone);
            }
        }
    }
    free(line);
    for (size_t i = 0; i < count; i++) {
        free(contacts[i].name);
        free(contacts[i].email);
        free(contacts[i].phone);
    }
    free(contacts);
    return 0;
}
