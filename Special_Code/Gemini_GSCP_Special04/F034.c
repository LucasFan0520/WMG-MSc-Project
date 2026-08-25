/* F034.c */
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
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    if (len > 0 && buf[len - 1] == '\n') {
        len--;
    }
    buf[len] = '\0';
    return buf;
}

char *mystrdup(const char *s) {
    size_t l = strlen(s);
    char *d = malloc(l + 1);
    if (d) strcpy(d, s);
    return d;
}

typedef struct {
    char *name;
    char *note;
} Note;

int main(int argc, char **argv) {
    if (argc < 2) return 1;
    Note *store = NULL;
    size_t count = 0;
    size_t cap = 0;
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
        if (*p == ' ') {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "LOAD") == 0) {
            FILE *fp = fopen(argv[1], "r");
            if (fp) {
                Note *new_store = NULL;
                size_t new_count = 0;
                size_t new_cap = 0;
                int valid = 1;
                while (1) {
                    char *fline = read_line(fp);
                    if (!fline) break;
                    char *fp_ptr = fline;
                    while (*fp_ptr == ' ') fp_ptr++;
                    if (*fp_ptr != '\0') {
                        char *name = fp_ptr;
                        while (*fp_ptr && *fp_ptr != ' ') fp_ptr++;
                        if (*fp_ptr == ' ') { *fp_ptr = '\0'; fp_ptr++; }
                        while (*fp_ptr == ' ') fp_ptr++;
                        char *note = fp_ptr;
                        if (strlen(name) == 0) {
                            valid = 0;
                        } else {
                            if (new_count >= new_cap) {
                                new_cap = new_cap == 0 ? 4 : new_cap * 2;
                                Note *ns = realloc(new_store, new_cap * sizeof(Note));
                                if (ns) new_store = ns;
                            }
                            new_store[new_count].name = mystrdup(name);
                            new_store[new_count].note = mystrdup(note);
                            new_count++;
                        }
                    }
                    free(fline);
                }
                fclose(fp);
                if (valid) {
                    for (size_t i = 0; i < count; i++) {
                        free(store[i].name);
                        free(store[i].note);
                    }
                    free(store);
                    store = new_store;
                    count = new_count;
                    cap = new_cap;
                } else {
                    for (size_t i = 0; i < new_count; i++) {
                        free(new_store[i].name);
                        free(new_store[i].note);
                    }
                    free(new_store);
                }
            }
        } else if (strcmp(cmd, "ADD") == 0) {
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *note = p;
            if (count >= cap) {
                cap = cap == 0 ? 4 : cap * 2;
                Note *ns = realloc(store, cap * sizeof(Note));
                if (ns) store = ns;
            }
            char *nn = mystrdup(name);
            char *nt = mystrdup(note);
            if (nn && nt) {
                store[count].name = nn;
                store[count].note = nt;
                count++;
            } else {
                free(nn); free(nt);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') { *p = '\0'; p++; }
            size_t i = 0;
            while (i < count) {
                if (strcmp(store[i].name, name) == 0) {
                    free(store[i].name);
                    free(store[i].note);
                    for (size_t j = i; j < count - 1; j++) {
                        store[j] = store[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s\n", store[i].name, store[i].note);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(store[i].name);
        free(store[i].note);
    }
    free(store);
    return 0;
}
