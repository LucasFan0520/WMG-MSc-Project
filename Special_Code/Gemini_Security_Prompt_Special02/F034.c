// F034.c
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
    char *note;
} StoreNote;

int main(int argc, char **argv) {
    StoreNote *items = NULL;
    size_t count = 0;
    size_t cap = 0;
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
        char *name = p;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        while (*p == ' ') p++;
        char *note = p;
        if (strcmp(cmd, "LOAD") == 0) {
            if (argc > 1) {
                FILE *fp = fopen(argv[1], "r");
                if (fp) {
                    StoreNote *new_items = NULL;
                    size_t new_count = 0;
                    size_t new_cap = 0;
                    int success = 1;
                    while (1) {
                        char *fline = read_line(fp);
                        if (!fline) break;
                        char *fp_line = fline;
                        while (*fp_line == ' ') fp_line++;
                        char *fname = fp_line;
                        while (*fp_line && *fp_line != ' ') fp_line++;
                        if (*fp_line) {
                            *fp_line = '\0';
                            fp_line++;
                        }
                        while (*fp_line == ' ') fp_line++;
                        char *fnote = fp_line;
                        if (strlen(fname) > 0 && strlen(fnote) > 0) {
                            if (new_count >= new_cap) {
                                new_cap = new_cap == 0 ? 4 : new_cap * 2;
                                new_items = realloc(new_items, new_cap * sizeof(StoreNote));
                            }
                            new_items[new_count].name = mystrdup(fname);
                            new_items[new_count].note = mystrdup(fnote);
                            new_count++;
                        }
                        free(fline);
                    }
                    fclose(fp);
                    if (success) {
                        for (size_t i = 0; i < count; i++) {
                            free(items[i].name);
                            free(items[i].note);
                        }
                        free(items);
                        items = new_items;
                        count = new_count;
                        cap = new_cap;
                    } else {
                        for (size_t i = 0; i < new_count; i++) {
                            free(new_items[i].name);
                            free(new_items[i].note);
                        }
                        free(new_items);
                    }
                }
            }
        } else if (strcmp(cmd, "ADD") == 0) {
            if (count >= cap) {
                cap = cap == 0 ? 4 : cap * 2;
                items = realloc(items, cap * sizeof(StoreNote));
            }
            items[count].name = mystrdup(name);
            items[count].note = mystrdup(note);
            count++;
        } else if (strcmp(cmd, "DELETE") == 0) {
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    free(items[i].name);
                    free(items[i].note);
                    for (size_t j = i; j < count - 1; j++) {
                        items[j] = items[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s\n", items[i].name, items[i].note);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(items[i].name);
        free(items[i].note);
    }
    free(items);
    return 0;
}
