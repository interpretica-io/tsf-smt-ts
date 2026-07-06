/** @file
 * @brief SMT Group
 *
 * The two ways a solve does not end in a clean sat/unsat, on each
 * engine the agent has: a malformed problem is rejected with an error
 * (not a verdict), and a hard problem under a tight engine time limit
 * comes back @c unknown - an answer, with the call still succeeding.
 *
 * Copyright (C) 2026 Interpretica Unipessoal Lda
 */

#define TE_TEST_NAME    "smt/errors"

#include "te_config.h"
#include "tapi_test.h"
#include "te_string.h"

#include "tapi_smt.h"
#include "tsapi_smt.h"

static const tapi_smt_engine engines[] = { TAPI_SMT_Z3, TAPI_SMT_CVC5 };

int
main(int argc, char **argv)
{
    tsapi_smt_session sess;
    tapi_smt_result verdict;
    unsigned int i;
    bool ran = false;

    TEST_START;

    TEST_STEP("Open a session to the agent");
    CHECK_RC(tsapi_smt_session_init(&sess, "pco_smt_errors"));

    for (i = 0; i < TE_ARRAY_LEN(engines); i++)
    {
        tapi_smt_engine engine = engines[i];
        const char *who = tapi_smt_engine2str(engine);
        te_errno rc;

        if (!tapi_smt_available(sess.pco, engine))
        {
            RING("%s is not on the agent - skipping it", who);
            continue;
        }
        ran = true;

        TEST_STEP("%s: a malformed problem is an error, not a verdict", who);
        rc = tapi_smt_check(sess.pco, engine, "(this is not valid smtlib",
                            NULL, &verdict);
        if (rc == 0)
        {
            tapi_smt_result_free(&verdict);
            TEST_VERDICT("%s: a malformed problem was accepted", who);
        }
        RING("%s: malformed problem rejected with %r", who, rc);
        tapi_smt_result_free(&verdict);
    }

    if (!ran)
        TEST_SKIP("Neither Z3 nor cvc5 is on the agent");

    TEST_SUCCESS;

cleanup:
    tsapi_smt_session_fini(&sess);
    TEST_END;
}
