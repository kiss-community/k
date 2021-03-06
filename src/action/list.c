/* SPDX-License-Identifier: MIT
 * Copyright (C) 2020 Dylan Araps
**/
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "action.h"
#include "arr.h"
#include "buf.h"
#include "error.h"
#include "file.h"
#include "pkg.h"
#include "repo.h"
#include "util.h"

static int db_to_list(struct state *s, const char *db) {
    DIR *d = opendir(db);

    if (!d) {
        err_no("failed to open database");
        return -1;
    }

    for (struct dirent *dp; (dp = read_dir(d)); ) {
        if (state_init_pkg(s, dp->d_name) < 0) {
            closedir(d);
            return -1;
        }
    }

    arr_sort(s->pkgs, pkg_sort_name);
    closedir(d);
    return 0;
}

static FILE *open_version(int fd, const char *pkg) {
    FILE *ver = fopenatat(fd, pkg, "version", O_RDONLY, "r");

    if (!ver) {
        if (errno == ENOENT) {
            err_no("package '%s' not installed", pkg);

        } else {
            err_no("[%s] failed to open version file", pkg);
        }

        return NULL;
    }

    return ver;
}

static int read_version(struct state *s, int fd, const char *pkg) {
    FILE *f = open_version(fd, pkg);

    if (!f) {
        return -1;
    }

    int err = buf_getline(&s->mem, f, 32);

    if (err < 0) {
        err_no("[%s] failed to read version file", pkg);
    }

    fclose(f);
    return err < 0 ? -1 : 0;
}

int action_list(struct state *s) {
    struct repo *db = repo_open_db("installed");

    if (!db) {
        err_no("failed to open database");
        return -1;
    }

    int err = 0;

    if (arr_len(s->pkgs) == 0 && (err = db_to_list(s, db->path)) < 0) {
        err_no("failed to read database");
        goto error;
    }

    for (size_t i = 0; i < arr_len(s->pkgs); i++) {
        buf_set_len(s->mem, 0);

        if ((err = read_version(s, db->fd, s->pkgs[i]->name)) < 0) {
            goto error;
        }

        size_t rel = buf_scan(&s->mem, 0, ' ');

        fprintf(stdout, "%s %s %s\n",
            s->pkgs[i]->name, s->mem, s->mem + rel);
    }

error:
    repo_free(db);
    return err;
}

