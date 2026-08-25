/* F015.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *email;
    char *phone;
    char *note;
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

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') line[len - 1] = '\0';
        if (strcmp(line, "END") == 0) break;

        char cmd[64] = {0};
        int offset = 0;
        if (sscanf(line, "%63s%n", cmd, &offset) != 1) continue;

        if (strcmp(cmd, "ADD") == 0) {
            char name[256] = {0}, email[256] = {0}, phone[256] = {0};
            if (sscanf(line + offset, " %255s %255s %255s", name, email, phone) != 3) continue;
            if (find_contact(name) >= 0) continue;
            if (ensure_cap() != 0) continue;
            contacts[ccount].name = dup_str(name);
            contacts[ccount].email = dup_str(email);
            contacts[ccount].phone = dup_str(phone);
            contacts[ccount].note = dup_str("");
            if (!contacts[ccount].name || !contacts[ccount].email ||
                !contacts[ccount].phone || !contacts[ccount].note) {
                free(contacts[ccount].name);
                free(contacts[ccount].email);
                free(contacts[ccount].phone);
                free(contacts[ccount].note);
                continue;
            }
            ccount++;
        } else if (strcmp(cmd, "NOTE") == 0) {
            char name[256] = {0};
            int off2 = 0;
            if (sscanf(line + offset, " %255s%n", name, &off2) != 1) continue;
            const char *text = line + offset + off2;
            while (*text == ' ') text++;
            int idx = find_contact(name);
            if (idx < 0) continue;
            char *nn = dup_str(text);
            if (!nn) continue;
            free(contacts[idx].note);
            contacts[idx].note = nn;
        } else if (strcmp(cmd, "MERGE") == 0) {
            char keep[256] = {0}, remove[256] = {0};
            if (sscanf(line + offset, " %255s %255s", keep, remove) != 2) continue;
            int ki = find_contact(keep);
            int ri = find_contact(remove);
            if (ki < 0 || ri < 0) continue;
            size_t kl = strlen(contacts[ki].note);
            size_t rl = strlen(contacts[ri].note);
            char *merged = malloc(kl + 1 + rl + 1);
            if (!merged) continue;
            memcpy(merged, contacts[ki].note, kl);
            if (kl > 0 && rl > 0) {
                merged[kl] = ' ';
                memcpy(merged + kl + 1, contacts[ri].note, rl + 1);
            } else {
                memcpy(merged + kl, contacts[ri].note, rl + 1);
            }
            free(contacts[ki].note);
            contacts[ki].note = merged;
            free(contacts[ri].name);
            free(contacts[ri].email);
            free(contacts[ri].phone);
            free(contacts[ri].note);
            for (int i = ri; i < ccount - 1; i++)
                contacts[i] = contacts[i + 1];
            ccount--;
        } else if (strcmp(cmd, "DELETE") == 0) {
            char name[256] = {0};
            if (sscanf(line + offset, " %255s", name) != 1) continue;
            int idx = find_contact(name);
            if (idx < 0) continue;
            free(contacts[idx].name);
            free(contacts[idx].email);
            free(contacts[idx].phone);
            free(contacts[idx].note);
            for (int i = idx; i < ccount - 1; i++)
                contacts[i] = contacts[i + 1];
            ccount--;
        } else if (strcmp(cmd, "FIND") == 0) {
            char name[256] = {0};
            if (sscanf(line + offset, " %255s", name) != 1) continue;
            int idx = find_contact(name);
            if (idx >= 0) {
                printf("%s %s %s %s\n", contacts[idx].name, contacts[idx].email,
                       contacts[idx].phone, contacts[idx].note);
            } else {
                printf("NOT_FOUND\n");
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < ccount; i++) {
                printf("%s %s %s %s\n", contacts[i].name, contacts[i].email,
                       contacts[i].phone, contacts[i].note);
            }
        }
    }
    for (int i = 0; i < ccount; i++) {
        free(contacts[i].name);
        free(contacts[i].email);
        free(contacts[i].phone);
        free(contacts[i].note);
    }
    free(contacts);
    return 0;
}