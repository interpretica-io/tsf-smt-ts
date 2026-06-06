/** @file
 * @brief SMT Group
 *
 * Proving a conjecture by refutation on each engine the agent has: a
 * valid entailment comes back @c unsat (the conjecture proved); an
 * invalid one comes back @c sat, and the model is the counter-example
 * that breaks it.
 *
 * Copyright (C) 2026 Interpretica Unipessoal Lda
 */

#define TE_TEST_NAME    "smt/prove"

#include "te_config.h"
#include "tapi_test.h"
#include "te_string.h"

#include "tapi_smt.h"
#include "tsapi_smt.h"

static const tapi_smt_engine engines[] = { TAPI_SMT_Z3, TAPI_SMT_CVC5 };

/** x > 3 over the integers, the shared assumption of both checks. */
#define ASSUMPTIONS \
    "(set-logic QF_LIA)\n"  \
    "(declare-const x Int)\n" \
    "(assert (> x 3))\n"

int
main(int argc, char **argv)
{
    tsapi_smt_session sess;
    tapi_smt_result result;
    unsigned int i;
    bool ran = false;

    TEST_START;

    TEST_STEP("Open a session to the agent");
    CHECK_RC(tsapi_smt_session_init(&sess, "pco_smt_prove"));

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

        TEST_STEP("%s: x > 2 follows from x > 3 (proved: unsat)", who);
        CHECK_RC(tapi_smt_prove(sess.pco, engine, ASSUMPTIONS, "(> x 2)",
                                NULL, &result));
        tapi_smt_result_log(&result);
        if (result.status != TAPI_SMT_UNSAT)
            TEST_VERDICT("%s: x > 2 should be proved, got %s", who,
                         tapi_smt_status2str(result.status));
        tapi_smt_result_free(&result);

        TEST_STEP("%s: x > 5 does NOT follow from x > 3 (sat, with a "
                  "counter-model)", who);
        opts.produce_model = true;
        CHECK_RC(tapi_smt_prove(sess.pco, engine, ASSUMPTIONS, "(> x 5)",
                                &opts, &result));
        tapi_smt_result_log(&result);
        if (result.status != TAPI_SMT_SAT)
            TEST_VERDICT("%s: x > 5 should not be proved, got %s", who,
                         tapi_smt_status2str(result.status));
        /* The witness must satisfy the assumption and break the
         * conjecture: 3 < x <= 5. */
        value = tapi_smt_get(&result, "x");
        if (value == NULL)
            TEST_VERDICT("%s: the counter-model has no x", who);
        RING("%s: counter-example x = %s", who, value);
        tapi_smt_result_free(&result);
    }

    if (!ran)
        TEST_SKIP("Neither Z3 nor cvc5 is on the agent");

    TEST_SUCCESS;

cleanup:
    tsapi_smt_session_fini(&sess);
    TEST_END;
}
