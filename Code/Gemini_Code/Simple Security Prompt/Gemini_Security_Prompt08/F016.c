// F016.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            char *nb = realloc(buf, cap);
            if (!nb) { free(buf); return NULL; }
            buf = nb;
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

int main(int argc, char **argv) {
    if (argc < 2) return 1;
    FILE *f = fopen(argv[1], "r");
    if (!f) return 1;
    Contact *list = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line(f);
        if (!line) break;
        char *c1 = strchr(line, ',');
        if (!c1) { free(line); continue; }
        char *c2 = strchr(c1 + 1, ',');
        if (!c2) { free(line); continue; }
        *c1 = '\0';
        *c2 = '\0';
        char *name = line;
        char *email = c1 + 1;
        char *phone = c2 + 1;
        if (count >= cap) {
            cap = cap == 0 ? 4 : cap * 2;
            Contact *nb = realloc(list, cap * sizeof(Contact));
            if (nb) list = nb;
        }
        if (count < cap) {
            list[count].name = strdup(name);
            list[count].email = strdup(email);
            list[count].phone = strdup(phone);
            count++;
        }
        free(line);
    }
    fclose(f);
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        if (*p == '\0') { free(line); continue; }
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p) { *p = '\0'; p++; }
        if (strcmp(cmd, "END") == 0) { free(line); break; }
        while (*p == ' ') p++;
        char *arg1 = p;
        if (strcmp(cmd, "ADD") == 0) {
            char *c1 = strchr(arg1, ',');
            if (c1) {
                char *c2 = strchr(c1 + 1, ',');
                if (c2) {
                    *c1 = '\0';
                    *c2 = '\0';
                    char *name = arg1;
                    char *email = c1 + 1;
                    char *phone = c2 + 1;
                    if (count >= cap) {
                        cap = cap == 0 ? 4 : cap * 2;
                        Contact *nb = realloc(list, cap * sizeof(Contact));
                        if (nb) list = nb;
                    }
                    if (count < cap) {
                        list[count].name = strdup(name);
                        list[count].email = strdup(email);
                        list[count].phone = strdup(phone);
                        count++;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].name, arg1) == 0) {
                    free(list[i].name);
                    free(list[i].email);
                    free(list[i].phone);
                    for (size_t j = i; j < count - 1; j++) {
                        list[j] = list[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].name, arg1) == 0) {
                    printf("%s,%s,%s\n", list[i].name, list[i].email, list[i].phone);
                    break;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s,%s,%s\n", list[i].name, list[i].email, list[i].phone);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(list[i].name);
        free(list[i].email);
        free(list[i].phone);
    }
    free(list);
    return 0;
}
