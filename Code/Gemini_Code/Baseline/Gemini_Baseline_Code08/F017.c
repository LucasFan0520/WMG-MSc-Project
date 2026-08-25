// F017.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char *value;
} ConfigPair;

ConfigPair *config = NULL;
int count = 0;
int capacity = 0;

int find_key(const char *key) {
    for (int i = 0; i < count; i++) {
        if (strcmp(config[i].key, key) == 0) {
            return i;
        }
    }
    return -1;
}

void set_kv(const char *key, const char *value) {
    int idx = find_key(key);
    if (idx != -1) {
        free(config[idx].value);
        config[idx].value = strdup(value);
        return;
    }
    if (count >= capacity) {
        capacity = capacity == 0 ? 4 : capacity * 2;
        config = realloc(config, capacity * sizeof(ConfigPair));
    }
    config[count].key = strdup(key);
    config[count].value = strdup(value);
    count++;
}

void unset_kv(const char *key) {
    int idx = find_key(key);
    if (idx != -1) {
        free(config[idx].key);
        free(config[idx].value);
        for (int i = idx; i < count - 1; i++) {
            config[i] = config[i + 1];
        }
        count--;
    }
}

void get_kv(const char *key) {
    int idx = find_key(key);
    if (idx != -1) {
        printf("%s\n", config[idx].value);
    }
}

void write_kv(void) {
    for (int i = 0; i < count; i++) {
        printf("%s=%s\n", config[i].key, config[i].value);
    }
}

int main(int argc, char *argv[]) {
    if (argc > 1) {
        FILE *fp = fopen(argv[1], "r");
        if (fp) {
            char *file_line = NULL;
            size_t file_len = 0;
            ssize_t file_read;
            while ((file_read = getline(&file_line, &file_len, fp)) != -1) {
                if (file_read > 0 && file_line[file_read - 1] == '\n') {
                    file_line[file_read - 1] = '\0';
                }
                char *eq = strchr(file_line, '=');
                if (eq) {
                    *eq = '\0';
                    set_kv(file_line, eq + 1);
                }
            }
            free(file_line);
            fclose(fp);
        }
    }
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
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "SET") == 0) {
            char *key = strtok(NULL, " ");
            char *value = strtok(NULL, "");
            if (key && value) set_kv(key, value);
        } else if (strcmp(cmd, "UNSET") == 0) {
            char *key = strtok(NULL, " ");
            if (key) unset_kv(key);
        } else if (strcmp(cmd, "GET") == 0) {
            char *key = strtok(NULL, " ");
            if (key) get_kv(key);
        } else if (strcmp(cmd, "WRITE") == 0) {
            write_kv();
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(config[i].key);
        free(config[i].value);
    }
    free(config);
    return 0;
}
