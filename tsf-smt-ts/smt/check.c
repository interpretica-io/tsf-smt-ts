/** @file
 * @brief SMT Group
 *
 * Deciding a set of assertions on each engine the agent has: a
 * satisfiable problem is @c sat with a model whose values satisfy it,
 * an unsatisfiable one is @c unsat.
 *
 * Copyright (C) 2026 Interpretica Unipessoal Lda
 */

#define TE_TEST_NAME    "smt/check"

#include "te_config.h"
#include "tapi_test.h"
#include "te_string.h"

#include "tapi_smt.h"
#include "tsapi_smt.h"

/** The engines this suite tries, each skipped when the agent lacks it. */
static const tapi_smt_engine engines[] = { TAPI_SMT_Z3, TAPI_SMT_CVC5 };

/** Decide one problem and check the verdict is @p want. */
static void
check_one(rcf_rpc_server *pco, tapi_smt_engine engine, const char *smtlib2,
          const tapi_smt_opts *opts, tapi_smt_status want,
          tapi_smt_result *result)
{
    CHECK_RC(tapi_smt_check(pco, engine, smtlib2, opts, result));
    tapi_smt_result_log(result);
    if (result->status != want)
    {
        TEST_VERDICT("%s: expected %s, got %s",
                     tapi_smt_engine2str(engine), tapi_smt_status2str(want),
                     tapi_smt_status2str(result->status));
    }
}

int
main(int argc, char **argv)
{
    tsapi_smt_session sess;
    tapi_smt_result result;
    unsigned int i;
    bool ran = false;

    TEST_START;

    TEST_STEP("Open a session to the agent");
    CHECK_RC(tsapi_smt_session_init(&sess, "pco_smt_check"));

    for (i = 0; i < TE_ARRAY_LEN(engines); i++)
    {
        tapi_smt_engine engine = engines[i];
        tapi_smt_opts opts = TAPI_SMT_OPTS_INIT;
        const char *who = tapi_smt_engine2str(engine);
        const char *value;

        if (!tapi_smt_available(sess.pco, engine))
        {
            RING("%s is not on the agent - skipping it", who);
            continue;
        }
        ran = true;

        TEST_STEP("%s: a satisfiable problem is sat with a usable model", who);
        opts.produce_model = true;
        check_one(sess.pco, engine,
                  "(set-logic QF_LIA)\n"
                  "(declare-const x Int)\n"
                  "(assert (> x 3))\n"
                  "(assert (< x 5))\n",
                  &opts, TAPI_SMT_SAT, &result);
        /* 3 < x < 5 over the integers has exactly one answer: x = 4. */
        value = tapi_smt_get(&result, "x");
        if (value == NULL)
            TEST_VERDICT("%s: the model has no x", who);
        if (strcmp(value, "4") != 0)
            TEST_VERDICT("%s: x = %s, expected 4", who, value);
        tapi_smt_result_free(&result);

        TEST_STEP("%s: an unsatisfiable problem is unsat", who);
        check_one(sess.pco, engine,
                  "(set-logic QF_LIA)\n"
                  "(declare-const x Int)\n"
                  "(assert (> x 3))\n"
                  "(assert (< x 2))\n",
                  NULL, TAPI_SMT_UNSAT, &result);
        tapi_smt_result_free(&result);
    }

    if (!ran)
        TEST_SKIP("Neither Z3 nor cvc5 is on the agent");

    TEST_SUCCESS;

cleanup:
    tsapi_smt_session_fini(&sess);
    TEST_END;
}
