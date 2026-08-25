// F016.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t cap = 128;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
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

char *mystrdup(const char *s) {
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (d) strcpy(d, s);
    return d;
}

typedef struct {
    char *name;
    char *email;
    char *phone;
} CSVContact;

int parse_csv_line(char *line, char **name, char **email, char **phone) {
    char *c1 = strchr(line, ',');
    if (!c1) return 0;
    *c1 = '\0';
    char *c2 = strchr(c1 + 1, ',');
    if (!c2) return 0;
    *c2 = '\0';
    *name = line;
    *email = c1 + 1;
    *phone = c2 + 1;
    return 1;
}

int main(int argc, char **argv) {
    CSVContact *items = NULL;
    size_t count = 0;
    size_t cap = 0;
    if (argc > 1) {
        FILE *fp = fopen(argv[1], "r");
        if (fp) {
            while (1) {
                char *line = read_line(fp);
                if (!line) break;
                char *n, *e, *p;
                if (parse_csv_line(line, &n, &e, &p)) {
                    if (count >= cap) {
                        cap = cap == 0 ? 4 : cap * 2;
                        items = realloc(items, cap * sizeof(CSVContact));
                    }
                    items[count].name = mystrdup(n);
                    items[count].email = mystrdup(e);
                    items[count].phone = mystrdup(p);
                    count++;
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
        while (*p == ' ') p++;
        char *arg = p;
        if (strcmp(cmd, "ADD") == 0) {
            char *n, *e, *p_num;
            if (parse_csv_line(arg, &n, &e, &p_num)) {
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    items = realloc(items, cap * sizeof(CSVContact));
                }
                items[count].name = mystrdup(n);
                items[count].email = mystrdup(e);
                items[count].phone = mystrdup(p_num);
                count++;
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, arg) == 0) {
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
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, arg) == 0) {
                    printf("%s,%s,%s\n", items[i].name, items[i].email, items[i].phone);
                    break;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s,%s,%s\n", items[i].name, items[i].email, items[i].phone);
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
