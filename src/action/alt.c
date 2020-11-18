/* SPDX-License-Identifier: MIT
 * Copyright (C) 2020 Dylan Araps
**/
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include "action.h"
#include "arr.h"
#include "download.h"
#include "error.h"
#include "file.h"
#include "pkg.h"
#include "sha256.h"
#include "util.h"

static void print_alt(const char *n) {
    for (size_t f = 0; *n; n++) {
        if (*n == '>') {
            if (!f++) {
                putchar(' ');
            }

            putchar('/');

        } else {
            putchar(*n);
        }
    }

    putchar('\n');
}

static int list_alts(struct state *s, const char *db) {
    DIR *d = opendir(db);

    if (!d) {
        err_no("failed to open alt database");
        return -1;
    }

    for (struct dirent *dp; (dp = read_dir(d)); ) {
        arr_push_b(s->argv, dp->d_name);
    }

    arr_sort(s->argv, qsort_cb_str);

    for (size_t i = 0; i < arr_len(s->argv); i++) {
        print_alt(s->argv[i]);
    }

    closedir(d);
    return 0;
}

int action_alt(struct state *s) {
    struct repo *db = repo_open_db("choices");

    if (!db) {
        err_no("failed to open alt database");
        return -1;
    }

    int ret = 0;

    if (arr_len(s->argv) == 0) {
        ret = list_alts(s, db->path);

    } else if (arr_len(s->argv) == 2) {
        // swap alts

    } else {
        err_no("invalid arguments");
        ret = -1;
    }

    repo_free(db);
    return ret;
}
