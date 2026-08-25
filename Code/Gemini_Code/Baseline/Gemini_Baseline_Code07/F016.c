// F016.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t size = 1024;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_buf = realloc(buf, size);
            if (!new_buf) { free(buf); return NULL; }
            buf = new_buf;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) { free(buf); return NULL; }
    buf[len] = '\0';
    return buf;
}

typedef struct {
    char *name;
    char *email;
    char *phone;
} Contact;

int parse_csv_line(char *line, char **name, char **email, char **phone) {
    char *c1 = strchr(line, ',');
    if (!c1) return 0;
    char *c2 = strchr(c1 + 1, ',');
    if (!c2) return 0;
    *name = malloc(c1 - line + 1);
    memcpy(*name, line, c1 - line);
    (*name)[c1 - line] = '\0';
    *email = malloc(c2 - c1);
    memcpy(*email, c1 + 1, c2 - c1 - 1);
    (*email)[c2 - c1 - 1] = '\0';
    *phone = strdup(c2 + 1);
    return 1;
}

int main(int argc, char **argv) {
    if (argc < 2) return 1;
    FILE *fp = fopen(argv[1], "r");
    Contact *contacts = NULL;
    int count = 0;
    int capacity = 0;
    if (fp) {
        while (1) {
            char *line = read_line(fp);
            if (!line) break;
            char *n = NULL, *e = NULL, *p = NULL;
            if (parse_csv_line(line, &n, &e, &p)) {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    contacts = realloc(contacts, capacity * sizeof(Contact));
                }
                contacts[count].name = n;
                contacts[count].email = e;
                contacts[count].phone = p;
                count++;
            }
            free(line);
        }
        fclose(fp);
    }
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char cmd[64];
        int n2 = 0;
        if (sscanf(line, "%63s%n", cmd, &n2) < 1) {
            free(line);
            continue;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s,%s,%s\n", contacts[i].name, contacts[i].email, contacts[i].phone);
            }
            free(line);
            continue;
        }
        char *p = line + n2;
        while (*p == ' ' || *p == '\t') p++;
        if (strcmp(cmd, "ADD") == 0) {
            char *n = NULL, *e = NULL, *p_str = NULL;
            if (parse_csv_line(p, &n, &e, &p_str)) {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    contacts = realloc(contacts, capacity * sizeof(Contact));
                }
                contacts[count].name = n;
                contacts[count].email = e;
                contacts[count].phone = p_str;
                count++;
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char name[256];
            if (sscanf(p, "%255s", name) == 1) {
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(contacts[i].name, name) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    free(contacts[found].name);
                    free(contacts[found].email);
                    free(contacts[found].phone);
                    for (int i = found; i < count - 1; i++) {
                        contacts[i] = contacts[i + 1];
                    }
                    count--;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char name[256];
            if (sscanf(p, "%255s", name) == 1) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(contacts[i].name, name) == 0) {
                        printf("%s,%s,%s\n", contacts[i].name, contacts[i].email, contacts[i].phone);
                        break;
                    }
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(contacts[i].name);
        free(contacts[i].email);
        free(contacts[i].phone);
    }
    free(contacts);
    return 0;
}
