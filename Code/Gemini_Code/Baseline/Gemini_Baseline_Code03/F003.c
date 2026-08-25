// F003.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *expansion;
} Alias;

int main() {
    Alias *table = NULL;
    int count = 0;
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        int len = strlen(line);
        while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r')) {
            line[len - 1] = '\0';
            len--;
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        char cmd[32];
        int read_bytes = 0;
        if (sscanf(line, "%31s%n", cmd, &read_bytes) <= 0) {
            continue;
        }
        char *ptr = line + read_bytes;
        while (*ptr == ' ') ptr++;
        if (strcmp(cmd, "DEFINE") == 0) {
            char name[256];
            int name_len = 0;
            if (sscanf(ptr, "%255s%n", name, &name_len) > 0) {
                char *exp = ptr + name_len;
                while (*exp == ' ') exp++;
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(table[i].name, name) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    free(table[found].expansion);
                    table[found].expansion = strdup(exp);
                } else {
                    table = realloc(table, (count + 1) * sizeof(Alias));
                    table[count].name = strdup(name);
                    table[count].expansion = strdup(exp);
                    count++;
                }
            }
        } else if (strcmp(cmd, "EXPAND") == 0) {
            char name[256];
            if (sscanf(ptr, "%255s", name) > 0) {
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(table[i].name, name) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    char *t = table[found].expansion;
                    while (*t) {
                        if (*t == ' ') putchar('_');
                        else putchar(*t);
                        t++;
                    }
                    putchar('\n');
                } else {
                    printf("NOT_FOUND\n");
                }
            }
        } else if (strcmp(cmd, "RENAME") == 0) {
            char old_name[256], new_name[256];
            if (sscanf(ptr, "%255s %255s", old_name, new_name) == 2) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(table[i].name, old_name) == 0) {
                        free(table[i].name);
                        table[i].name = strdup(new_name);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "ERASE") == 0) {
            char name[256];
            if (sscanf(ptr, "%255s", name) > 0) {
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(table[i].name, name) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    free(table[found].name);
                    free(table[found].expansion);
                    for (int i = found; i < count - 1; i++) {
                        table[i] = table[i + 1];
                    }
                    count--;
                    if (count > 0) {
                        table = realloc(table, count * sizeof(Alias));
                    } else {
                        free(table);
                        table = NULL;
                    }
                }
            }
        } else if (strcmp(cmd, "DUMP") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s\n", table[i].name, table[i].expansion);
            }
        }
    }
    for (int i = 0; i < count; i++) {
        free(table[i].name);
        free(table[i].expansion);
    }
    free(table);
    return 0;
}
