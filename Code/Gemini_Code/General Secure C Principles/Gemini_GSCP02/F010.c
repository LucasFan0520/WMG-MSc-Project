// F010.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char *value;
} Entry;

int main(void) {
    Entry *index = NULL;
    int count = 0;
    char *line = NULL;
    size_t cap = 0;
    ssize_t read;
    while ((read = getline(&line, &cap, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char k[256], v[256];
            if (sscanf(line + 4, "%255s %255s", k, v) == 2) {
                Entry *tmp = realloc(index, (count + 1) * sizeof(Entry));
                if (tmp) {
                    index = tmp;
                    index[count].key = strdup(k);
                    index[count].value = strdup(v);
                    count++;
                }
            }
        } else if (strncmp(line, "REMOVE ", 7) == 0) {
            char k[256], v[256];
            if (sscanf(line + 7, "%255s %255s", k, v) == 2) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(index[i].key, k) == 0 && strcmp(index[i].value, v) == 0) {
                        free(index[i].key);
                        free(index[i].value);
                        for (int j = i; j < count - 1; j++) {
                            index[j] = index[j + 1];
                        }
                        count--;
                        break;
                    }
                }
            }
        } else if (strncmp(line, "ERASE ", 6) == 0) {
            char *k = line + 6;
            while (*k == ' ') {
                k++;
            }
            int i = 0;
            while (i < count) {
                if (strcmp(index[i].key, k) == 0) {
                    free(index[i].key);
                    free(index[i].value);
                    for (int j = i; j < count - 1; j++) {
                        index[j] = index[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strncmp(line, "VALUES ", 7) == 0) {
            char *k = line + 7;
            while (*k == ' ') {
                k++;
            }
            for (int i = 0; i < count; i++) {
                if (strcmp(index[i].key, k) == 0) {
                    printf("%s\n", index[i].value);
                }
            }
        } else if (strcmp(line, "KEYS") == 0) {
            char **seen = NULL;
            int scount = 0;
            for (int i = 0; i < count; i++) {
                int found = 0;
                for (int j = 0; j < scount; j++) {
                    if (strcmp(seen[j], index[i].key) == 0) {
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    printf("%s\n", index[i].key);
                    char **tmp = realloc(seen, (scount + 1) * sizeof(char *));
                    if (tmp) {
                        seen = tmp;
                        seen[scount] = index[i].key;
                        scount++;
                    }
                }
            }
            free(seen);
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(index[i].key);
        free(index[i].value);
    }
    free(index);
    return 0;
}
