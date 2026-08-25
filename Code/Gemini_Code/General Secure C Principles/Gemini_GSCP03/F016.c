// F016.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            char *nb = realloc(buf, cap);
            if (!nb) {
                free(buf);
                return NULL;
            }
            buf = nb;
        }
        buf[len++] = c;
    }
    if (c == EOF && len == 0) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

typedef struct {
    char *name;
    char *email;
    char *phone;
} Contact;

Contact *items = NULL;
size_t count = 0;
size_t cap = 0;

void add_contact(char *name, char *email, char *phone) {
    if (count >= cap) {
        cap = cap == 0 ? 16 : cap * 2;
        Contact *nb = realloc(items, cap * sizeof(Contact));
        if (nb) items = nb;
    }
    items[count].name = strdup(name);
    items[count].email = strdup(email);
    items[count].phone = strdup(phone);
    count++;
}

int main(int argc, char **argv) {
    if (argc >= 2) {
        FILE *fp = fopen(argv[1], "r");
        if (fp) {
            while (1) {
                char *line = read_line(fp);
                if (!line) break;
                char *comma1 = strchr(line, ',');
                if (comma1) {
                    char *comma2 = strchr(comma1 + 1, ',');
                    if (comma2) {
                        *comma1 = '\0';
                        *comma2 = '\0';
                        char *name = line;
                        char *email = comma1 + 1;
                        char *phone = comma2 + 1;
                        size_t pl = strlen(phone);
                        if (pl > 0 && phone[pl - 1] == '\r') phone[pl - 1] = '\0';
                        add_contact(name, email, phone);
                    }
                }
                free(line);
            }
            fclose(fp);
        }
    }
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s,%s,%s\n", items[i].name, items[i].email, items[i].phone);
            }
            free(line);
            continue;
        }
        while (*p == ' ') p++;
        if (strcmp(cmd, "ADD") == 0) {
            char *comma1 = strchr(p, ',');
            if (comma1) {
                char *comma2 = strchr(comma1 + 1, ',');
                if (comma2) {
                    *comma1 = '\0';
                    *comma2 = '\0';
                    char *name = p;
                    char *email = comma1 + 1;
                    char *phone = comma2 + 1;
                    add_contact(name, email, phone);
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = p;
            for (size_t i = 0; i < count; ) {
                if (strcmp(items[i].name, name) == 0) {
                    free(items[i].name);
                    free(items[i].email);
                    free(items[i].phone);
                    for (size_t j = i; j < count - 1; j++) {
                        items[j] = items[j + 1];
                    }
                    count--;
                } else {
                    i++;
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
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(items[i].name);
        free(items[i].email);
        free(items[i].phone);
    }
    free(items);
    return 0;
}
