// F036.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Record {
    char *name;
    char *data;
    struct Record *next;
} Record;

Record *head = NULL;

Record *find_record(const char *name) {
    Record *r = head;
    while (r) {
        if (strcmp(r->name, name) == 0) return r;
        r = r->next;
    }
    return NULL;
}

void add_record(const char *name, const char *data) {
    if (find_record(name)) return;
    Record *r = malloc(sizeof(Record));
    if (!r) return;
    r->name = strdup(name);
    r->data = strdup(data);
    r->next = NULL;
    if (!r->name || !r->data) {
        free(r->name); free(r->data); free(r);
        return;
    }
    if (!head) head = r;
    else {
        Record *last = head;
        while (last->next) last = last->next;
        last->next = r;
    }
}

void delete_record(const char *name) {
    Record *r = head, *prev = NULL;
    while (r) {
        if (strcmp(r->name, name) == 0) {
            if (prev) prev->next = r->next;
            else head = r->next;
            free(r->name);
            free(r->data);
            free(r);
            return;
        }
        prev = r;
        r = r->next;
    }
}

void show_record(const char *name) {
    Record *r = find_record(name);
    if (!r) return;
    for (char *c = r->data; *c; c++) {
        putchar(*c == ' ' ? '_' : *c);
    }
    putchar('\n');
}

void report_names(void) {
    Record *r = head;
    while (r) {
        printf("%s\n", r->name);
        r = r->next;
    }
}

void free_all(void) {
    Record *r = head;
    while (r) {
        Record *next = r->next;
        free(r->name);
        free(r->data);
        free(r);
        r = next;
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[64], name[256], data[4096];
        if (sscanf(line, "%63s", cmd) != 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            // parse name and length:data
            char *colon = strchr(line, ':');
            if (!colon) continue;
            char *space = strchr(line, ' ');
            if (!space) continue;
            // skip command
            char *start = space + 1;
            // name is until next space? but name may contain no spaces
            char *name_end = strchr(start, ' ');
            if (!name_end) continue;
            int namelen = name_end - start;
            if (namelen >= 255) continue;
            strncpy(name, start, namelen);
            name[namelen] = '\0';
            // parse length
            int length = 0;
            char *len_start = name_end + 1;
            if (*len_start < '0' || *len_start > '9') continue;
            while (*len_start >= '0' && *len_start <= '9') {
                length = length * 10 + (*len_start - '0');
                len_start++;
                if (length > 4096) break;
            }
            if (*len_start != ':') continue;
            char *data_start = len_start + 1;
            if (length > 0 && strlen(data_start) < (size_t)length) continue;
            strncpy(data, data_start, length);
            data[length] = '\0';
            add_record(name, data);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1)
                delete_record(name);
        } else if (strcmp(cmd, "SHOW") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1)
                show_record(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_names();
        }
    }
    free(line);
    free_all();
    return 0;
}