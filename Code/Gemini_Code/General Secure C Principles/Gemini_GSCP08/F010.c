// F010.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t cap = 128;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            size_t ncap = cap * 2;
            char *nb = realloc(buf, ncap);
            if (!nb) {
                free(buf);
                return NULL;
            }
            buf = nb;
            cap = ncap;
        }
        buf[len++] = c;
    }
    if (c == EOF && len == 0) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

typedef struct {
    char *key;
    char **values;
    size_t val_count;
    size_t val_cap;
} KeyList;

int main(void) {
    KeyList *arr = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        char *cmd = line;
        char *p1 = strchr(cmd, ' ');
        if (!p1) {
            if (strcmp(cmd, "KEYS") == 0) {
                for (size_t i = 0; i < count; i++) {
                    printf("%s\n", arr[i].key);
                }
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *args = p1 + 1;
        if (strcmp(cmd, "ADD") == 0) {
            char *key = args;
            char *p2 = strchr(key, ' ');
            if (p2) {
                *p2 = '\0';
                char *val = p2 + 1;
                int found = -1;
                for (size_t i = 0; i < count; i++) {
                    if (strcmp(arr[i].key, key) == 0) {
                        found = (int)i;
                        break;
                    }
                }
                if (found == -1) {
                    if (count >= cap) {
                        size_t ncap = cap == 0 ? 4 : cap * 2;
                        KeyList *narr = realloc(arr, ncap * sizeof(KeyList));
                        if (narr) {
                            arr = narr;
                            cap = ncap;
                        }
                    }
                    if (count < cap) {
                        arr[count].key = strdup(key);
                        arr[count].values = NULL;
                        arr[count].val_count = 0;
                        arr[count].val_cap = 0;
                        found = (int)count;
                        count++;
                    }
                }
                if (found != -1) {
                    KeyList *kl = &arr[found];
                    if (kl->val_count >= kl->val_cap) {
                        size_t ncap = kl->val_cap == 0 ? 4 : kl->val_cap * 2;
                        char **nvals = realloc(kl->values, ncap * sizeof(char *));
                        if (nvals) {
                            kl->values = nvals;
                            kl->val_cap = ncap;
                        }
                    }
                    if (kl->val_count < kl->val_cap) {
                        kl->values[kl->val_count] = strdup(val);
                        kl->val_count++;
                    }
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *key = args;
            char *p2 = strchr(key, ' ');
            if (p2) {
                *p2 = '\0';
                char *val = p2 + 1;
                for (size_t i = 0; i < count; i++) {
                    if (strcmp(arr[i].key, key) == 0) {
                        for (size_t j = 0; j < arr[i].val_count; j++) {
                            if (strcmp(arr[i].values[j], val) == 0) {
                                free(arr[i].values[j]);
                                for (size_t k = j; k < arr[i].val_count - 1; k++) {
                                    arr[i].values[k] = arr[i].values[k + 1];
                                }
                                arr[i].val_count--;
                                break;
                            }
                        }
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "ERASE") == 0) {
            char *key = args;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].key, key) == 0) {
                    free(arr[i].key);
                    for (size_t j = 0; j < arr[i].val_count; j++) {
                        free(arr[i].values[j]);
                    }
                    free(arr[i].values);
                    for (size_t j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "VALUES") == 0) {
            char *key = args;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].key, key) == 0) {
                    for (size_t j = 0; j < arr[i].val_count; j++) {
                        printf("%s%s", arr[i].values[j], j == arr[i].val_count - 1 ? "" : " ");
                    }
                    printf("\n");
                    break;
                }
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(arr[i].key);
        for (size_t j = 0; j < arr[i].val_count; j++) {
            free(arr[i].values[j]);
        }
        free(arr[i].values);
    }
    free(arr);
    return 0;
}
