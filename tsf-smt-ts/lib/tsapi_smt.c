/* SPDX-License-Identifier: MIT */
/* Copyright (C) 2026 Interpretica, Unipessoal Lda. All rights reserved. */
/** @file
 * @brief Suite helpers
 *
 * @author Maxim Menshikov <maxim.menshikov@interpretica.io>
 */

#define TE_LGR_USER "TSAPI SMT"

#include "te_config.h"

#include <string.h>

#include "logger_api.h"
#include "te_alloc.h"

#include "tsapi_smt.h"

/* See description in tsapi_smt.h */
te_errno
tsapi_smt_session_init(tsapi_smt_session *session, const char *name)
{
    te_errno rc;

    memset(session, 0, sizeof(*session));
    session->ta = TSAPI_SMT_TA;

    rc = rcf_rpc_server_create(session->ta, name, &session->pco);
    if (rc != 0)
    {
        ERROR("Cannot create the RPC server '%s' on %s: %r", name,
              session->ta, rc);
        return rc;
    }

    return 0;
}

/* See description in tsapi_smt.h */
void
tsapi_smt_session_fini(tsapi_smt_session *session)
{
    if (session->pco != NULL)
    {
        rcf_rpc_server_destroy(session->pco);
        session->pco = NULL;
    }
}
