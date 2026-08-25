/* F010.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char **values;
    int val_count;
    int val_cap;
} KeyList;

int main(void) {
    KeyList *index = NULL;
    int count = 0;
    int capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *args = line + 4;
            char *space = strchr(args, ' ');
            if (!space) continue;
            *space = '\0';
            char *k = args;
            char *v = space + 1;
            int idx = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(index[i].key, k) == 0) {
                    idx = i;
                    break;
                }
            }
            if (idx == -1) {
                if (count >= capacity) {
                    int new_cap = capacity == 0 ? 4 : capacity * 2;
                    KeyList *tmp = realloc(index, new_cap * sizeof(KeyList));
                    if (tmp) {
                        index = tmp;
                        capacity = new_cap;
                    } else {
                        *space = ' ';
                        continue;
                    }
                }
                char *n_key = strdup(k);
                if (n_key) {
                    index[count].key = n_key;
                    index[count].values = NULL;
                    index[count].val_count = 0;
                    index[count].val_cap = 0;
                    idx = count;
                    count++;
                }
            }
            if (idx != -1) {
                char *n_val = strdup(v);
                if (n_val) {
                    KeyList *kl = &index[idx];
                    if (kl->val_count >= kl->val_cap) {
                        int new_vcap = kl->val_cap == 0 ? 4 : kl->val_cap * 2;
                        char **tmp = realloc(kl->values, new_vcap * sizeof(char *));
                        if (tmp) {
                            kl->values = tmp;
                            kl->val_cap = new_vcap;
                        } else {
                            free(n_val);
                            *space = ' ';
                            continue;
                        }
                    }
                    kl->values[kl->val_count] = n_val;
                    kl->val_count++;
                }
            }
            *space = ' ';
        } else if (strncmp(line, "REMOVE ", 7) == 0) {
            char *args = line + 7;
            char *space = strchr(args, ' ');
            if (!space) continue;
            *space = '\0';
            char *k = args;
            char *v = space + 1;
            for (int i = 0; i < count; i++) {
                if (strcmp(index[i].key, k) == 0) {
                    KeyList *kl = &index[i];
                    for (int j = 0; j < kl->val_count; j++) {
                        if (strcmp(kl->values[j], v) == 0) {
                            free(kl->values[j]);
                            for (int m = j; m < kl->val_count - 1; m++) {
                                kl->values[m] = kl->values[m + 1];
                            }
                            kl->val_count--;
                            break;
                        }
                    }
                    break;
                }
            }
            *space = ' ';
        } else if (strncmp(line, "ERASE ", 6) == 0) {
            char *k = line + 6;
            for (int i = 0; i < count; i++) {
                if (strcmp(index[i].key, k) == 0) {
                    free(index[i].key);
                    for (int j = 0; j < index[i].val_count; j++) {
                        free(index[i].values[j]);
                    }
                    free(index[i].values);
                    for (int j = i; j < count - 1; j++) {
                        index[j] = index[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "VALUES ", 7) == 0) {
            char *k = line + 7;
            for (int i = 0; i < count; i++) {
                if (strcmp(index[i].key, k) == 0) {
                    for (int j = 0; j < index[i].val_count; j++) {
                        printf("%s\n", index[i].values[j]);
                    }
                    break;
                }
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
        for (int j = 0; j < index[i].val_count; j++) {
            free(index[i].values[j]);
        }
        free(index[i].values);
    }
    free(index);
    return 0;
}
