Tests for each syscall are subdirs within their respective syscall directories.
E.g. the simple test for pwritev resides in `./pwritev/test_simple/`. Run `make`
within that directory to build the test. The code for the test is in 
`./pwritev/test_simple/pwritev_test.c`, and the executable is 
`./pwritev/test_simple/pwritev_test` (after running `make` of course).
`test_complete` tests run more comprehensive tests, while `test_simple`tests
keep to a small set of primitive syscalls (e.g. `open`, `close`, `read`,
`write`) to keep syscall tests simple and easy to understand.