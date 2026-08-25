// F003.c
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
    char *expansion;
} Alias;

int main() {
    Alias *aliases = NULL;
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
        if (strcmp(cmd, "DUMP") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s\n", aliases[i].name, aliases[i].expansion);
            }
            free(line);
            continue;
        }
        char *p = line + n;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        if (strcmp(cmd, "DEFINE") == 0) {
            char *name_start = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            int name_len = p - name_start;
            char *name = malloc(name_len + 1);
            memcpy(name, name_start, name_len);
            name[name_len] = '\0';
            while (*p == ' ' || *p == '\t') p++;
            char *expansion = p;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(aliases[i].name, name) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                free(aliases[found].expansion);
                aliases[found].expansion = strdup(expansion);
                free(name);
            } else {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    aliases = realloc(aliases, capacity * sizeof(Alias));
                }
                aliases[count].name = name;
                aliases[count].expansion = strdup(expansion);
                count++;
            }
        } else if (strcmp(cmd, "EXPAND") == 0) {
            char name[256];
            if (sscanf(p, "%255s", name) == 1) {
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(aliases[i].name, name) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    for (int i = 0; aliases[found].expansion[i]; i++) {
                        if (aliases[found].expansion[i] == ' ') {
                            printf("_");
                        } else {
                            printf("%c", aliases[found].expansion[i]);
                        }
                    }
                    printf("\n");
                }
            }
        } else if (strcmp(cmd, "RENAME") == 0) {
            char old_name[256], new_name[256];
            if (sscanf(p, "%255s %255s", old_name, new_name) == 2) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(aliases[i].name, old_name) == 0) {
                        free(aliases[i].name);
                        aliases[i].name = strdup(new_name);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "ERASE") == 0) {
            char name[256];
            if (sscanf(p, "%255s", name) == 1) {
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(aliases[i].name, name) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    free(aliases[found].name);
                    free(aliases[found].expansion);
                    for (int i = found; i < count - 1; i++) {
                        aliases[i] = aliases[i + 1];
                    }
                    count--;
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(aliases[i].name);
        free(aliases[i].expansion);
    }
    free(aliases);
    return 0;
}
