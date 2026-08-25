// F016.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *email;
    char *phone;
} Contact;

void parse_csv_line(char *line, char **name, char **email, char **phone) {
    char *c1 = strchr(line, ',');
    if (!c1) return;
    char *c2 = strchr(c1 + 1, ',');
    if (!c2) return;

    *c1 = '\0';
    *c2 = '\0';
    *name = line;
    *email = c1 + 1;
    *phone = c2 + 1;
}

int main(int argc, char **argv) {
    if (argc < 2) return 1;

    Contact *items = NULL;
    size_t count = 0;
    size_t capacity = 0;

    FILE *f = fopen(argv[1], "r");
    if (f) {
        char *fline = NULL;
        size_t flen = 0;
        long fread_bytes;
        while ((fread_bytes = getline(&fline, &flen, f)) != -1) {
            if (fread_bytes > 0 && fline[fread_bytes - 1] == '\n') {
                fline[fread_bytes - 1] = '\0';
                fread_bytes--;
            }
            if (fread_bytes > 0 && fline[fread_bytes - 1] == '\n') {
                fline[fread_bytes - 1] = '\0';
                fread_bytes--;
            }

            char *name = NULL;
            char *email = NULL;
            char *phone = NULL;
            parse_csv_line(fline, &name, &email, &phone);

            if (name && email && phone) {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    Contact *new_items = realloc(items, capacity * sizeof(Contact));
                    if (!new_items) exit(1);
                    items = new_items;
                }
                items[count].name = strdup(name);
                items[count].email = strdup(email);
                items[count].phone = strdup(phone);
                if (!items[count].name || !items[count].email || !items[count].phone) exit(1);
                count++;
            }
        }
        free(fline);
        fclose(f);
    }

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
            char *name = NULL;
            char *email = NULL;
            char *phone = NULL;
            parse_csv_line(p, &name, &email, &phone);

            if (name && email && phone) {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    Contact *new_items = realloc(items, capacity * sizeof(Contact));
                    if (!new_items) exit(1);
                    items = new_items;
                }
                items[count].name = strdup(name);
                items[count].email = strdup(email);
                items[count].phone = strdup(phone);
                if (!items[count].name || !items[count].email || !items[count].phone) exit(1);
                count++;
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = p;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    free(items[i].name);
                    free(items[i].email);
                    free(items[i].phone);
                    for (size_t j = i; j < count - 1; j++) {
                        items[j] = items[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *name = p;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    printf("%s,%s,%s\n", items[i].name, items[i].email, items[i].phone);
                    break;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s,%s,%s\n", items[i].name, items[i].email, items[i].phone);
            }
        }
    }

    for (size_t i = 0; i < count; i++) {
        free(items[i].name);
        free(items[i].email);
        free(items[i].phone);
    }
    free(items);
    free(line);
    return 0;
}
