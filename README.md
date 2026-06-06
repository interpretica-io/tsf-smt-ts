# tsf-smt-ts

A Test Environment suite that exercises
[tsf-smt](https://github.com/interpretica-io/tsf-smt) (`tapi_smt`)
against the agent it runs on — deciding SMT-LIB 2 problems with Z3 and
cvc5, with no key and no network (the solver is a library linked into
the agent's RPC server).

| Test | What it checks |
|---|---|
| `check` | a satisfiable problem (`3 < x < 5` over the integers) is `sat` with a model, and `tapi_smt_get()` reads `x = 4`; an unsatisfiable one is `unsat` |
| `prove` | `x > 2` follows from `x > 3` → `unsat` (proved); `x > 5` does not → `sat` with a counter-model whose `x` breaks it |
| `core` | two jointly contradictory named assertions give a non-empty unsat core; Z3 names it by label (`a`, `b`), cvc5 by the asserted term (checked per engine) |
| `errors` | a malformed problem is rejected with an error (not a verdict); a nonlinear problem under a 5 ms engine limit comes back `unknown` with the call still succeeding |

Every group runs against **both engines** and skips one the agent does
not have (`tapi_smt_available()`), so the suite is green on a host that
carries only one of Z3/cvc5, and fully exercised on one that carries
both. There is no mock and no network: the solver runs in the agent's
RPC server, called through its own library API (Z3's C API, cvc5's C++
API), so each problem is decided locally and deterministically.

## Running it

Needs Docker and `test-environment` as a sibling directory:

```bash
./scripts/run.sh docker guess --cfg=localhost
```

On the `localhost` configuration the agent is the build container, so
that container must carry **Z3 and cvc5 with their development headers**
(`libz3-dev`, cvc5 with `cvc5/cvc5.h` and `-lcvc5`) and a **C++
compiler** — see the agent host requirements in the tsf-smt README.
Name a host in `conf/rcf.conf` and the same tests run against it.

`conf/external.yml` names the tsf-smt repository and the ref to follow;
`conf/builder.conf.lock` pins the commit actually built (empty until
the first build, since tsf-smt is not published yet). The Builder
clones tsf-smt itself, so the checkout beside this suite is not what a
run compiles.

## Status

**Not yet run.** This suite was written alongside tsf-smt but has not
been built or executed — there was no TE toolchain, and tsf-smt itself
has not been compiled (its C/C++ was written to the tsf-upnp template;
the solving behaviour was validated through the engines' Python
bindings). The first run should expect the ordinary first-build fixes,
and in particular the cvc5 C++ API version pin (see the tsf-smt README).
The test logic here — what each engine should answer — is the same that
was validated against Z3 4.16.0 and cvc5 1.4.1.
