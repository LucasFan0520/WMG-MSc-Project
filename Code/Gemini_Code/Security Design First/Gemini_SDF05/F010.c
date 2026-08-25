// F010.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char **values;
    int v_count;
    int v_capacity;
} KeyList;

int main(void) {
    KeyList *index = NULL;
    int count = 0;
    int capacity = 0;
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
            while (*p == ' ') p++;
            char *key = p;
            char *space = strchr(key, ' ');
            if (!space) continue;
            *space = '\0';
            char *val = space + 1;
            while (*val == ' ') val++;
            
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(index[i].key, key) == 0) {
                    found = i;
                    break;
                }
            }
            if (found == -1) {
                if (count >= capacity) {
                    int new_cap = capacity == 0 ? 4 : capacity * 2;
                    KeyList *tmp = realloc(index, new_cap * sizeof(KeyList));
                    if (!tmp) return 1;
                    index = tmp;
                    capacity = new_cap;
                }
                char *nk = strdup(key);
                if (!nk) return 1;
                index[count].key = nk;
                index[count].values = NULL;
                index[count].v_count = 0;
                index[count].v_capacity = 0;
                found = count;
                count++;
            }
            KeyList *kl = &index[found];
            if (kl->v_count >= kl->v_capacity) {
                int new_vcap = kl->v_capacity == 0 ? 4 : kl->v_capacity * 2;
                char **tmp = realloc(kl->values, new_vcap * sizeof(char *));
                if (!tmp) return 1;
                kl->values = tmp;
                kl->v_capacity = new_vcap;
            }
            char *nv = strdup(val);
            if (!nv) return 1;
            kl->values[kl->v_count] = nv;
            kl->v_count++;
        } else if (strncmp(line, "REMOVE ", 7) == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *key = p;
            char *space = strchr(key, ' ');
            if (!space) continue;
            *space = '\0';
            char *val = space + 1;
            while (*val == ' ') val++;
            
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(index[i].key, key) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                KeyList *kl = &index[found];
                int vf = -1;
                for (int j = 0; j < kl->v_count; j++) {
                    if (strcmp(kl->values[j], val) == 0) {
                        vf = j;
                        break;
                    }
                }
                if (vf != -1) {
                    free(kl->values[vf]);
                    for (int j = vf; j < kl->v_count - 1; j++) {
                        kl->values[j] = kl->values[j + 1];
                    }
                    kl->v_count--;
                }
            }
        } else if (strncmp(line, "ERASE ", 6) == 0) {
            char *key = line + 6;
            while (*key == ' ') key++;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(index[i].key, key) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                free(index[found].key);
                for (int j = 0; j < index[found].v_count; j++) free(index[found].values[j]);
                free(index[found].values);
                for (int i = found; i < count - 1; i++) {
                    index[i] = index[i + 1];
                }
                count--;
            }
        } else if (strncmp(line, "VALUES ", 7) == 0) {
            char *key = line + 7;
            while (*key == ' ') key++;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(index[i].key, key) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                for (int j = 0; j < index[found].v_count; j++) {
                    printf("%s%s", index[found].values[j], (j == index[found].v_count - 1) ? "" : " ");
                }
                putchar('\n');
            }
        } else if (strcmp(line, "KEYS") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s\n", index[i].key);
            }
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(index[i].key);
        for (int j = 0; j < index[i].v_count; j++) free(index[i].values[j]);
        free(index[i].values);
    }
    free(index);
    return 0;
}
