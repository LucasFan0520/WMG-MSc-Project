/* F016.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *email;
    char *phone;
} Contact;

static Contact *contacts = NULL;
static int ccount = 0;
static int ccap = 0;

static int ensure_cap(void) {
    if (ccount >= ccap) {
        int newcap = ccap == 0 ? 8 : ccap * 2;
        Contact *tmp = realloc(contacts, sizeof(Contact) * newcap);
        if (!tmp) return -1;
        contacts = tmp;
        ccap = newcap;
    }
    return 0;
}

static char *dup_str(const char *s) {
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (d) memcpy(d, s, len + 1);
    return d;
}

static int find_contact(const char *name) {
    for (int i = 0; i < ccount; i++) {
        if (strcmp(contacts[i].name, name) == 0)
            return i;
    }
    return -1;
}

static int parse_csv_line(const char *line, char *name, char *email, char *phone, size_t sz) {
    const char *p = line;
    const char *c1 = strchr(p, ',');
    if (!c1) return -1;
    size_t l1 = (size_t)(c1 - p);
    if (l1 >= sz) return -1;
    memcpy(name, p, l1);
    name[l1] = '\0';
    p = c1 + 1;
    const char *c2 = strchr(p, ',');
    if (!c2) return -1;
    size_t l2 = (size_t)(c2 - p);
    if (l2 >= sz) return -1;
    memcpy(email, p, l2);
    email[l2] = '\0';
    p = c2 + 1;
    size_t l3 = strlen(p);
    if (l3 >= sz) return -1;
    memcpy(phone, p, l3);
    phone[l3] = '\0';
    return 0;
}

static void load_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    char line[4096];
    while (fgets(line, sizeof(line), f)) {
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') line[len - 1] = '\0';
        if (len > 1 && line[len - 2] == '\r') line[len - 2] = '\0';
        char name[256], email[256], phone[256];
        if (parse_csv_line(line, name, email, phone, 256) != 0) continue;
        if (strlen(name) == 0 || strlen(email) == 0 || strlen(phone) == 0) continue;
        if (ensure_cap() != 0) continue;
        contacts[ccount].name = dup_str(name);
        contacts[ccount].email = dup_str(email);
        contacts[ccount].phone = dup_str(phone);
        if (!contacts[ccount].name || !contacts[ccount].email || !contacts[ccount].phone) {
            free(contacts[ccount].name);
            free(contacts[ccount].email);
            free(contacts[ccount].phone);
            continue;
        }
        ccount++;
    }
    fclose(f);
}

int main(int argc, char *argv[]) {
    if (argc > 1) {
        load_file(argv[1]);
    }
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') line[len - 1] = '\0';
        if (strcmp(line, "END") == 0) break;

        char cmd[64] = {0};
        int offset = 0;
        if (sscanf(line, "%63s%n", cmd, &offset) != 1) continue;

        if (strcmp(cmd, "ADD") == 0) {
            const char *data = line + offset;
            while (*data == ' ') data++;
            char name[256], email[256], phone[256];
            if (parse_csv_line(data, name, email, phone, 256) != 0) continue;
            if (find_contact(name) >= 0) continue;
            if (ensure_cap() != 0) continue;
            contacts[ccount].name = dup_str(name);
            contacts[ccount].email = dup_str(email);
            contacts[ccount].phone = dup_str(phone);
            if (!contacts[ccount].name || !contacts[ccount].email || !contacts[ccount].phone) {
                free(contacts[ccount].name);
                free(contacts[ccount].email);
                free(contacts[ccount].phone);
                continue;
            }
            ccount++;
        } else if (strcmp(cmd, "DELETE") == 0) {
            char name[256] = {0};
            if (sscanf(line + offset, " %255s", name) != 1) continue;
            int idx = find_contact(name);
            if (idx < 0) continue;
            free(contacts[idx].name);
            free(contacts[idx].email);
            free(contacts[idx].phone);
            for (int i = idx; i < ccount - 1; i++)
                contacts[i] = contacts[i + 1];
            ccount--;
        } else if (strcmp(cmd, "FIND") == 0) {
            char name[256] = {0};
            if (sscanf(line + offset, " %255s", name) != 1) continue;
            int idx = find_contact(name);
            if (idx >= 0) {
                printf("%s,%s,%s\n", contacts[idx].name, contacts[idx].email, contacts[idx].phone);
            } else {
                printf("NOT_FOUND\n");
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < ccount; i++) {
                printf("%s,%s,%s\n", contacts[i].name, contacts[i].email, contacts[i].phone);
            }
        }
    }
    for (int i = 0; i < ccount; i++) {
        free(contacts[i].name);
        free(contacts[i].email);
        free(contacts[i].phone);
    }
    free(contacts);
    return 0;
}