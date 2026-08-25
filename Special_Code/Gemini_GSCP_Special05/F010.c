// F010.c
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
    char *key;
    char **values;
    size_t val_count;
    size_t val_cap;
} KeyList;

int main(void) {
    KeyList *index = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *p = line + 4;
            char *key = p;
            while (*p && *p != ' ') p++;
            char *val = "";
            if (*p == ' ') {
                *p = '\0';
                val = p + 1;
            }
            size_t found = (size_t)-1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(index[i].key, key) == 0) {
                    found = i;
                    break;
                }
            }
            if (found == (size_t)-1) {
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    KeyList *nb = realloc(index, cap * sizeof(KeyList));
                    if (!nb) {
                        free(line);
                        break;
                    }
                    index = nb;
                }
                index[count].key = safe_dup(key);
                index[count].values = NULL;
                index[count].val_count = 0;
                index[count].val_cap = 0;
                found = count;
                count++;
            }
            KeyList *kl = &index[found];
            if (kl->val_count >= kl->val_cap) {
                kl->val_cap = kl->val_cap == 0 ? 4 : kl->val_cap * 2;
                char **nb = realloc(kl->values, kl->val_cap * sizeof(char *));
                if (!nb) {
                    free(line);
                    break;
                }
                kl->values = nb;
            }
            kl->values[kl->val_count] = safe_dup(val);
            kl->val_count++;
        } else if (strncmp(line, "REMOVE ", 7) == 0) {
            char *p = line + 7;
            char *key = p;
            while (*p && *p != ' ') p++;
            char *val = "";
            if (*p == ' ') {
                *p = '\0';
                val = p + 1;
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(index[i].key, key) == 0) {
                    KeyList *kl = &index[i];
                    for (size_t j = 0; j < kl->val_count; j++) {
                        if (strcmp(kl->values[j], val) == 0) {
                            free(kl->values[j]);
                            for (size_t k = j; k < kl->val_count - 1; k++) {
                                kl->values[k] = kl->values[k + 1];
                            }
                            kl->val_count--;
                            break;
                        }
                    }
                    break;
                }
            }
        } else if (strncmp(line, "ERASE ", 6) == 0) {
            char *key = line + 6;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(index[i].key, key) == 0) {
                    free(index[i].key);
                    for (size_t j = 0; j < index[i].val_count; j++) {
                        free(index[i].values[j]);
                    }
                    free(index[i].values);
                    for (size_t j = i; j < count - 1; j++) {
                        index[j] = index[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "VALUES ", 7) == 0) {
            char *key = line + 7;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(index[i].key, key) == 0) {
                    for (size_t j = 0; j < index[i].val_count; j++) {
                        printf("%s%s", index[i].values[j], (j == index[i].val_count - 1) ? "" : " ");
                    }
                    printf("\n");
                    break;
                }
            }
        } else if (strcmp(line, "KEYS") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s\n", index[i].key);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(index[i].key);
        for (size_t j = 0; j < index[i].val_count; j++) {
            free(index[i].values[j]);
        }
        free(index[i].values);
    }
    free(index);
    return 0;
}
