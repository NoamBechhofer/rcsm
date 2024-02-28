#include <sys/uio.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/*
 * These two are declared in `unistd.h`, but my vim is being weird about finding
 * them so I'm just putting them here to make vim happy
 */
extern int getopt(int argc, char *const argv[], const char *opstring);
extern char *optarg;

// #define WRITEV_TEST_DEBUG

#ifdef WRITEV_TEST_DEBUG
#define writev_test_debug(dbg) \
	fprintf(stderr, #dbg " = %zu\n", dbg);
#else
#define writev_test_debug(dbg) 
#endif

#if SIZE_MAX == UINT_MAX || SIZE_MAX == ULONG_MAX
	#define STRTOSIZE_T(nptr, endptr, base) strtoul(nptr, endptr, base)
#elif SIZE_MAX == ULLONG_MAX
	#define STRTOSIZE_T(nptr, endptr, base) strtoull(nptr, endptr, base)
#else
	#error "cannot determine function to use for converting string to size_t"
#endif

#define DEFAULT_NUM_BUFS 8
#define DEFAULT_BUF_LEN  8

/**
 * writev test described in the writev manpage on linux.die.net
 * @return 0 if successful, non-zero otherwise
 */
int writev_basic_test()
{
	char *str0 = "hello ";
	char *str1 = "world\n";
	size_t v_len = 2;
	struct iovec iov[2];
	ssize_t nwritten;

	iov[0].iov_base = str0;
	iov[0].iov_len = 6;
	iov[1].iov_base = str1;
	iov[1].iov_len = 6;

	nwritten = writev(STDOUT_FILENO, iov, v_len);

	if (nwritten < 6 + 6)
			return 1;

	return 0;
}

/**
 * regular writev test, where the number of buffers and length of them is
 * specified. Buffers are written to `/dev/null`.
 *
 * @return 0 if successful, non-zero otherwise
 */
int writev_parameterized_test(size_t num_bufs, size_t buf_len)
{
	struct iovec iovecs[num_bufs];

	size_t total_write = num_bufs * buf_len;
	if (total_write > 0x7FFFF000) {
		printf("cannot write more than 0x7FFFF000 bytes. "
				"You are trying to write 0x%zX bytes. "
				"Expect a partial write.\n", total_write);
	}

	printf("writing 0x%zX buffers, each of size 0x%zX (total 0x%zX bytes),"
			" to `/dev/null`\n", num_bufs, buf_len, total_write);

	for (int i = 0; i < num_bufs; i++) {
		iovecs[i].iov_base = malloc(buf_len);
		iovecs[i].iov_len = buf_len;

		/* 
		 * Per the man page, malloc might return NULL for requests for
		 * 0 bytes.
		 */
		if (iovecs[i].iov_base == NULL && buf_len != 0) {
			perror("writev_parameterized_test: could not allocate "
					"enough memory to fill iovec buffers");
			return 1;
		}
	}

	int fd = open("/dev/null", O_WRONLY);
	if (fd == -1) {
		perror("writev_parameterized_test: could not open `/dev/null`");
		return 1;
	}

	ssize_t nwritten = writev(fd, iovecs, num_bufs);

	printf("wrote 0x%zX bytes successfully\n", nwritten);

	for (int i = 0; i < num_bufs; i++) {
		free(iovecs[i].iov_base);
	}
	return 0;
}

int main(int argc, char **argv)
{
	size_t num_bufs = DEFAULT_NUM_BUFS, buf_len = DEFAULT_BUF_LEN;

	/*
	 * Use getopt to parse command line arguments. This allows us to
	 * override the default values for `num_bufs` and/or `buf_len`
	 */
	int opt;
	while ((opt = getopt(argc, argv, "c:l:")) != -1) {
		switch (opt) {
		case 'c':
			num_bufs = STRTOSIZE_T(optarg, NULL, 10);
			writev_test_debug(num_bufs);
			break;
		case 'l':
			buf_len = STRTOSIZE_T(optarg, NULL, 10);
			writev_test_debug(buf_len);
			break;
		default:
			fprintf(stderr, "Usage: %s [-c num_bufs] [-l buf_len]\n",
					argv[0]);
			exit(1);
			break;

		}
	}
	/* finished parsing */

	/* run the test(s) */
	int ret = writev_parameterized_test(num_bufs, buf_len);
#ifdef WRITEV_TEST_DEBUG
	ret |= writev_basic_test();
#endif
	return ret;
}

