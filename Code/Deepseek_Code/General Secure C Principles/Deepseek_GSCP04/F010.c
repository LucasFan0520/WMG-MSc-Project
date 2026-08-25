/* F010.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct KeyVal {
    char *key;
    char **values;
    size_t vcount, vcap;
    struct KeyVal *next;
} KeyVal;

static KeyVal *list = NULL;

static KeyVal *find_key(const char *key) {
    KeyVal *kv = list;
    while (kv) {
        if (strcmp(kv->key, key) == 0) return kv;
        kv = kv->next;
    }
    return NULL;
}

static void add_value(KeyVal *kv, const char *val) {
    if (kv->vcount == kv->vcap) {
        kv->vcap = kv->vcap ? kv->vcap * 2 : 4;
        kv->values = realloc(kv->values, kv->vcap * sizeof(char *));
        if (!kv->values) { perror("realloc"); exit(1); }
    }
    kv->values[kv->vcount] = strdup(val);
    kv->vcount++;
}

static void add_key(const char *key) {
    KeyVal *kv = malloc(sizeof(KeyVal));
    if (!kv) { perror("malloc"); exit(1); }
    kv->key = strdup(key);
    kv->values = NULL;
    kv->vcount = kv->vcap = 0;
    kv->next = list;
    list = kv;
}

static void erase_key(const char *key) {
    KeyVal **curr = &list;
    while (*curr) {
        if (strcmp((*curr)->key, key) == 0) {
            KeyVal *kv = *curr;
            *curr = kv->next;
            free(kv->key);
            for (size_t i = 0; i < kv->vcount; i++) free(kv->values[i]);
            free(kv->values);
            free(kv);
            return;
        }
        curr = &(*curr)->next;
    }
}

static void remove_value(const char *key, const char *val) {
    KeyVal *kv = find_key(key);
    if (!kv) return;
    for (size_t i = 0; i < kv->vcount; i++) {
        if (strcmp(kv->values[i], val) == 0) {
            free(kv->values[i]);
            for (size_t j = i; j < kv->vcount - 1; j++)
                kv->values[j] = kv->values[j + 1];
            kv->vcount--;
            return;
        }
    }
}

static void values_key(const char *key) {
    KeyVal *kv = find_key(key);
    if (!kv) return;
    for (size_t i = 0; i < kv->vcount; i++) {
        printf("%s\n", kv->values[i]);
    }
}

static void keys_list(void) {
    KeyVal *kv = list;
    while (kv) {
        printf("%s\n", kv->key);
        kv = kv->next;
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *key = strtok(NULL, " ");
            if (!key) continue;
            char *val = strtok(NULL, "");
            if (!val) val = "";
            KeyVal *kv = find_key(key);
            if (!kv) { add_key(key); kv = find_key(key); }
            if (kv) add_value(kv, val);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *key = strtok(NULL, " ");
            char *val = strtok(NULL, " ");
            if (key && val) remove_value(key, val);
        } else if (strcmp(cmd, "ERASE") == 0) {
            char *key = strtok(NULL, " ");
            if (key) erase_key(key);
        } else if (strcmp(cmd, "VALUES") == 0) {
            char *key = strtok(NULL, " ");
            if (key) values_key(key);
        } else if (strcmp(cmd, "KEYS") == 0) {
            keys_list();
        }
    }
    // cleanup omitted
    return 0;
}