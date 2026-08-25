// F015.c
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
    Contact *items = NULL;
    size_t count = 0;
    size_t capacity = 0;
    char *line = NULL;
    size_t len = 0;
    long read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
            read--;
        }
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
            read--;
        }

        if (strcmp(line, "END") == 0) {
            break;
        }

        char *p = line;
        while (*p == ' ' || *p == '	') p++;
        if (*p == '\0') continue;

        char *cmd = p;
        while (*p && *p != ' ' && *p != '	') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        while (*p == ' ' || *p == '	') p++;

        if (strcmp(cmd, "ADD") == 0) {
            char *name = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '	') p++;

            char *email = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '	') p++;

            char *phone = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) *p = '\0';

            if (count >= capacity) {
                capacity = capacity == 0 ? 4 : capacity * 2;
                Contact *new_items = realloc(items, capacity * sizeof(Contact));
                if (!new_items) exit(1);
                items = new_items;
            }
            items[count].name = strdup(name);
            items[count].email = strdup(email);
            items[count].phone = strdup(phone);
            items[count].note = strdup("");
            if (!items[count].name || !items[count].email || !items[count].phone || !items[count].note) exit(1);
            count++;
        } else if (strcmp(cmd, "NOTE") == 0) {
            char *name = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '	') p++;
            char *note = p;

            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    free(items[i].note);
                    items[i].note = strdup(note);
                    if (!items[i].note) exit(1);
                    break;
                }
            }
        } else if (strcmp(cmd, "MERGE") == 0) {
            char *keep = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '	') p++;
            char *remove = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) *p = '\0';

            int idxK = -1, idxR = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, keep) == 0) idxK = (int)i;
                if (strcmp(items[i].name, remove) == 0) idxR = (int)i;
            }

            if (idxK != -1 && idxR != -1) {
                size_t l1 = strlen(items[idxK].note);
                size_t l2 = strlen(items[idxR].note);
                char *cn = malloc(l1 + l2 + 2);
                if (!cn) exit(1);
                strcpy(cn, items[idxK].note);
                if (l1 > 0 && l2 > 0) {
                    strcat(cn, " ");
                }
                strcat(cn, items[idxR].note);

                free(items[idxK].note);
                items[idxK].note = cn;

                free(items[idxR].name);
                free(items[idxR].email);
                free(items[idxR].phone);
                free(items[idxR].note);

                for (size_t j = (size_t)idxR; j < count - 1; j++) {
                    items[j] = items[j + 1];
                }
                count--;
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) *p = '\0';

            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    free(items[i].name);
                    free(items[i].email);
                    free(items[i].phone);
                    free(items[i].note);
                    for (size_t j = i; j < count - 1; j++) {
                        items[j] = items[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *name = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) *p = '\0';

            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    printf("%s %s %s Note: %s\n", items[i].name, items[i].email, items[i].phone, items[i].note);
                    break;
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s %s\n", items[i].name, items[i].email, items[i].phone);
            }
        }
    }

    for (size_t i = 0; i < count; i++) {
        free(items[i].name);
        free(items[i].email);
        free(items[i].phone);
        free(items[i].note);
    }
    free(items);
    free(line);
    return 0;
}
