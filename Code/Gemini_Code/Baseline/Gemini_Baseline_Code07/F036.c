// F036.c
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
    char *data;
} Record36;

int main() {
    Record36 *store = NULL;
    int count = 0;
    int capacity = 0;
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
                printf("%s\n", store[i].name);
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
                int len_val = 0;
                int n3 = 0;
                if (sscanf(p, "%d:%n", &len_val, &n3) == 1 && len_val >= 0) {
                    p += n3;
                    if ((int)strlen(p) >= len_val) {
                        char *data_buf = malloc(len_val + 1);
                        memcpy(data_buf, p, len_val);
                        data_buf[len_val] = '\0';
                        if (count >= capacity) {
                            capacity = capacity == 0 ? 4 : capacity * 2;
                            store = realloc(store, capacity * sizeof(Record36));
                        }
                        store[count].name = strdup(name);
                        store[count].data = data_buf;
                        count++;
                    }
                }
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
                    free(store[found].data);
                    for (int i = found; i < count - 1; i++) {
                        store[i] = store[i + 1];
                    }
                    count--;
                }
            }
        } else if (strcmp(cmd, "SHOW") == 0) {
            char name[256];
            if (sscanf(p, "%255s", name) == 1) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(store[i].name, name) == 0) {
                        for (int j = 0; store[i].data[j]; j++) {
                            if (store[i].data[j] == ' ') printf("_");
                            else printf("%c", store[i].data[j]);
                        }
                        printf("\n");
                        break;
                    }
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(store[i].name);
        free(store[i].data);
    }
    free(store);
    return 0;
}
