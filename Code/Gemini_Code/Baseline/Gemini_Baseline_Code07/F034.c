// F034.c
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
    char *note;
} Note34;

int main(int argc, char **argv) {
    Note34 *store = NULL;
    int count = 0;
    int capacity = 0;
    char *filepath = (argc > 1) ? argv[1] : NULL;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char cmd[64];
        int n = 0;
        if (sscanf(line, "%63s%n", cmd, &n) < 1) {
            free(line);
            continue;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s: %s\n", store[i].name, store[i].note);
            }
            free(line);
            continue;
        }
        if (strcmp(cmd, "LOAD") == 0) {
            if (filepath) {
                FILE *fp = fopen(filepath, "r");
                if (fp) {
                    Note34 *temp_store = NULL;
                    int temp_count = 0;
                    int temp_capacity = 0;
                    int valid = 1;
                    while (1) {
                        char *f_line = read_line(fp);
                        if (!f_line) break;
                        char name[256];
                        int n2 = 0;
                        if (sscanf(f_line, "%255s%n", name, &n2) == 1) {
                            char *note = f_line + n2;
                            while (*note == ' ' || *note == '\t') note++;
                            if (temp_count >= temp_capacity) {
                                temp_capacity = temp_capacity == 0 ? 4 : temp_capacity * 2;
                                temp_store = realloc(temp_store, temp_capacity * sizeof(Note34));
                            }
                            temp_store[temp_count].name = strdup(name);
                            temp_store[temp_count].note = strdup(note);
                            temp_count++;
                        } else {
                            valid = 0;
                            free(f_line);
                            break;
                        }
                        free(f_line);
                    }
                    fclose(fp);
                    if (valid) {
                        for (int i = 0; i < count; i++) {
                            free(store[i].name);
                            free(store[i].note);
                        }
                        free(store);
                        store = temp_store;
                        count = temp_count;
                        capacity = temp_capacity;
                    } else {
                        for (int i = 0; i < temp_count; i++) {
                            free(temp_store[i].name);
                            free(temp_store[i].note);
                        }
                        free(temp_store);
                    }
                }
            }
            free(line);
            continue;
        }
        char *p = line + n;
        while (*p == ' ' || *p == '\t') p++;
        if (strcmp(cmd, "ADD") == 0) {
            char name[256];
            int n2 = 0;
            if (sscanf(p, "%255s%n", name, &n2) == 1) {
                p += n2;
                while (*p == ' ' || *p == '\t') p++;
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    store = realloc(store, capacity * sizeof(Note34));
                }
                store[count].name = strdup(name);
                store[count].note = strdup(p);
                count++;
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char name[256];
            if (sscanf(p, "%255s", name) == 1) {
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(store[i].name, name) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    free(store[found].name);
                    free(store[found].note);
                    for (int i = found; i < count - 1; i++) {
                        store[i] = store[i + 1];
                    }
                    count--;
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(store[i].name);
        free(store[i].note);
    }
    free(store);
    return 0;
}
