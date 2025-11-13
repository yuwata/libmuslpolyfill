/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <glob.h>

#include "macro.h"

/* The header glob.h overrides glob with mpf_glob, hence we need to undef it here. */
#undef glob

_public_ int mpf_glob(
                const char *pattern,
                int flags,
                int (*errfunc)(const char *epath, int eerrno),
                glob_t *pglob) {

        return glob(pattern, flags, errfunc, pglob);
}
