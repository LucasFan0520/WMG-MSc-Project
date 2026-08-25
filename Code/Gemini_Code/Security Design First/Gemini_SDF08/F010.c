// F010.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char **values;
    int v_count;
} Entry;

int main(void) {
    Entry *entries = NULL;
    int count = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *p = line + 4;
            char *space = strchr(p, ' ');
            if (space) {
                *space = '\0';
                char *key = p;
                char *val = space + 1;
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(entries[i].key, key) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found == -1) {
                    Entry *tmp = realloc(entries, sizeof(Entry) * (count + 1));
                    if (tmp) {
                        entries = tmp;
                        entries[count].key = strdup(key);
                        entries[count].values = NULL;
                        entries[count].v_count = 0;
                        if (entries[count].key) {
                            found = count;
                            count++;
                        }
                    }
                }
                if (found != -1) {
                    char **v_tmp = realloc(entries[found].values, sizeof(char *) * (entries[found].v_count + 1));
                    if (v_tmp) {
                        entries[found].values = v_tmp;
                        entries[found].values[entries[found].v_count] = strdup(val);
                        if (entries[found].values[entries[found].v_count]) {
                            entries[found].v_count++;
                        }
                    }
                }
            }
        } else if (strncmp(line, "REMOVE ", 7) == 0) {
            char *p = line + 7;
            char *space = strchr(p, ' ');
            if (space) {
                *space = '\0';
                char *key = p;
                char *val = space + 1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(entries[i].key, key) == 0) {
                        int v_found = -1;
                        for (int j = 0; j < entries[i].v_count; j++) {
                            if (strcmp(entries[i].values[j], val) == 0) {
                                v_found = j;
                                break;
                            }
                        }
                        if (v_found != -1) {
                            free(entries[i].values[v_found]);
                            for (int j = v_found; j < entries[i].v_count - 1; j++) {
                                entries[i].values[j] = entries[i].values[j + 1];
                            }
                            entries[i].v_count--;
                        }
                        break;
                    }
                }
            }
        } else if (strncmp(line, "ERASE ", 6) == 0) {
            char *key = line + 6;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(entries[i].key, key) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                free(entries[found].key);
                for (int j = 0; j < entries[found].v_count; j++) {
                    free(entries[found].values[j]);
                }
                free(entries[found].values);
                for (int i = found; i < count - 1; i++) {
                    entries[i] = entries[i + 1];
                }
                count--;
            }
        } else if (strncmp(line, "VALUES ", 7) == 0) {
            char *key = line + 7;
            for (int i = 0; i < count; i++) {
                if (strcmp(entries[i].key, key) == 0) {
                    for (int j = 0; j < entries[i].v_count; j++) {
                        printf("%s\n", entries[i].values[j]);
                    }
                    break;
                }
            }
        } else if (strcmp(line, "KEYS") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s\n", entries[i].key);
            }
        }
    }
    for (int i = 0; i < count; i++) {
        free(entries[i].key);
        for (int j = 0; j < entries[i].v_count; j++) {
            free(entries[i].values[j]);
        }
        free(entries[i].values);
    }
    free(entries);
    free(line);
    return 0;
}
