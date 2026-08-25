/* F017.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char *value;
} Config;

static Config *configs = NULL;
static size_t ccount = 0, ccap = 0;

static Config *find_config(const char *key) {
    for (size_t i = 0; i < ccount; i++) {
        if (strcmp(configs[i].key, key) == 0)
            return &configs[i];
    }
    return NULL;
}

static void set_config(const char *key, const char *value) {
    Config *c = find_config(key);
    if (c) {
        free(c->value);
        c->value = strdup(value);
    } else {
        if (ccount == ccap) {
            ccap = ccap ? ccap * 2 : 4;
            configs = realloc(configs, ccap * sizeof(Config));
            if (!configs) { perror("realloc"); exit(1); }
        }
        configs[ccount].key = strdup(key);
        configs[ccount].value = strdup(value);
        ccount++;
    }
}

static void unset_config(const char *key) {
    for (size_t i = 0; i < ccount; i++) {
        if (strcmp(configs[i].key, key) == 0) {
            free(configs[i].key);
            free(configs[i].value);
            for (size_t j = i; j < ccount - 1; j++)
                configs[j] = configs[j + 1];
            ccount--;
            return;
        }
    }
}

static void get_config(const char *key) {
    Config *c = find_config(key);
    if (c) printf("%s\n", c->value);
}

static void write_configs(void) {
    for (size_t i = 0; i < ccount; i++) {
        printf("%s=%s\n", configs[i].key, configs[i].value);
    }
}

static void load_file(const char *fname) {
    FILE *f = fopen(fname, "r");
    if (!f) return;
    char line[4096];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = '\0';
        char *eq = strchr(line, '=');
        if (!eq) continue;
        *eq = '\0';
        char *key = line;
        char *value = eq + 1;
        set_config(key, value);
    }
    fclose(f);
}

int main(int argc, char **argv) {
    if (argc > 1) load_file(argv[1]);
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "SET") == 0) {
            char *key = strtok(NULL, " ");
            if (!key) continue;
            char *val = strtok(NULL, "");
            if (!val) val = "";
            set_config(key, val);
        } else if (strcmp(cmd, "UNSET") == 0) {
            char *key = strtok(NULL, " ");
            if (key) unset_config(key);
        } else if (strcmp(cmd, "GET") == 0) {
            char *key = strtok(NULL, " ");
            if (key) get_config(key);
        } else if (strcmp(cmd, "WRITE") == 0) {
            write_configs();
        }
    }
    for (size_t i = 0; i < ccount; i++) {
        free(configs[i].key);
        free(configs[i].value);
    }
    free(configs);
    return 0;
}