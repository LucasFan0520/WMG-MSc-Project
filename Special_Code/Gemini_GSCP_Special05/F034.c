// F034.c
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
        buf[len - 1] = '\0';
    } else {
        buf[len] = '\0';
    }
    return buf;
}

char *safe_dup(const char *s) {
    size_t l = strlen(s);
    char *d = malloc(l + 1);
    if (d) {
        strcpy(d, s);
    }
    return d;
}

typedef struct {
    char *name;
    char *note;
} NoteRecord;

int main(int argc, char **argv) {
    if (argc < 2) return 1;
    NoteRecord *store = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(line, "LOAD") == 0) {
            FILE *fp = fopen(argv[1], "r");
            if (fp) {
                NoteRecord *temp_store = NULL;
                size_t temp_count = 0;
                size_t temp_cap = 0;
                int load_ok = 1;
                while (1) {
                    char *fline = read_line(fp);
                    if (!fline) break;
                    char *p = fline;
                    while (*p && *p != ' ') p++;
                    if (*p == ' ' && p != fline) {
                        *p = '\0';
                        if (temp_count >= temp_cap) {
                            temp_cap = temp_cap == 0 ? 4 : temp_cap * 2;
                            NoteRecord *nb = realloc(temp_store, temp_cap * sizeof(NoteRecord));
                            if (!nb) { load_ok = 0; free(fline); break; }
                            temp_store = nb;
                        }
                        temp_store[temp_count].name = safe_dup(fline);
                        temp_store[temp_count].note = safe_dup(p + 1);
                        temp_count++;
                    } else {
                        load_ok = 0;
                        free(fline);
                        break;
                    }
                    free(fline);
                }
                fclose(fp);
                if (load_ok) {
                    for (size_t i = 0; i < count; i++) {
                        free(store[i].name);
                        free(store[i].note);
                    }
                    free(store);
                    store = temp_store;
                    count = temp_count;
                    cap = temp_cap;
                } else {
                    for (size_t i = 0; i < temp_count; i++) {
                        free(temp_store[i].name);
                        free(temp_store[i].note);
                    }
                    free(temp_store);
                }
            }
        } else if (strncmp(line, "ADD ", 4) == 0) {
            char *p = line + 4;
            char *name = p;
            while (*p && *p != ' ') p++;
            char *note = "";
            if (*p == ' ') {
                *p = '\0';
                note = p + 1;
            }
            if (count >= cap) {
                cap = cap == 0 ? 4 : cap * 2;
                NoteRecord *nb = realloc(store, cap * sizeof(NoteRecord));
                if (!nb) {
                    free(line);
                    break;
                }
                store = nb;
            }
            store[count].name = safe_dup(name);
            store[count].note = safe_dup(note);
            count++;
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *name = line + 7;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(store[i].name, name) == 0) {
                    free(store[i].name);
                    free(store[i].note);
                    for (size_t j = i; j < count - 1; j++) {
                        store[j] = store[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(line, "REPORT") == 0) {
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
