#define _XOPEN_SOURCE 500
#include <ftw.h>

#ifdef _WIN32
#include <io.h>
#else
#include <libgen.h>
#include <unistd.h>
#endif

#include "maxminddb_test_helper.h"

int test_read(const char *path, const struct stat *UNUSED(
                  sbuf), int flags, struct FTW *UNUSED(ftw))
{
    // Check if path is a regular file)
    if (flags != FTW_F) {
        return 0;
    }

    MMDB_s *mmdb = (MMDB_s *)calloc(1, sizeof(MMDB_s));

    if (NULL == mmdb) {
        BAIL_OUT("could not allocate memory for our MMDB_s struct");
    }

    int status = MMDB_open(path, MMDB_MODE_MMAP, mmdb);

    if (status != MMDB_SUCCESS) {
        ok(1, "received error when opening %s", path);
        return 0;
    }

    int gai_error, mmdb_error;
    MMDB_lookup_string(mmdb, "ip", &gai_error, &mmdb_error);

    cmp_ok(mmdb_error, "!=", MMDB_SUCCESS, "opening %s returned an error",
           path);

    MMDB_close(mmdb);
    free(mmdb);
    return 0;
}

int main(void)
{
    char *test_db_dir;
#ifdef _WIN32
    test_db_dir = "./t/maxmind-db/bad-data";
#else
    char cwd[500];
    char *UNUSED(tmp) = getcwd(cwd, 500);

    if (strcmp(basename(cwd), "t") == 0) {
        test_db_dir = "./maxmind-db/bad-data";
    } else {
        test_db_dir = "./t/maxmind-db/bad-data";
    }
#endif
    plan(NO_PLAN);
    if (nftw(test_db_dir, test_read, 10, FTW_PHYS) != 0) {
        BAIL_OUT("nftw failed");
    }
    done_testing();
}
