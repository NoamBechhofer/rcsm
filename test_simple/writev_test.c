#include <sys/uio.h>
#include <sys/mman.h>

#include <fcntl.h>
#include <unistd.h>

#include <string.h>

#define STDERR_FD 2

void usage(char *prog_name)
{
	char *usage_msg[] = {
		"Usage: ",
		prog_name,
		" <write_file>\n"
	};
	size_t usage_msg_segments = 3;

	for (size_t i = 0; i < usage_msg_segments; i++) {
		char *segment = usage_msg[i];
		if (segment == NULL)
			continue;
		size_t segment_len = strlen(segment);
		ssize_t nwritten = 0;
		while (*segment != '\0') {
			nwritten = write(STDERR_FD, segment, segment_len);
			segment_len -= nwritten;
			segment += nwritten;
		}
	}
}

/**
 * regular writev test
 * @return 0 if successful, non-zero otherwise
 */
int writev_test(char *write_file)
{
	char *iov_bufs[] = {
		"test ",
		"message ",
		"to ",
		"write ",
		"to ",
		"file\n"
	};
	size_t num_bufs = 6;

	int fd = open(write_file, O_WRONLY | O_CREAT, 0666);
	if (fd == -1) {
		char *err =  "error on open()\n";
		write(STDERR_FD, err, strlen(err));
		return 1;
	}

	struct iovec iovecs[num_bufs];

	for (int i = 0; i < num_bufs; i++) {
		iovecs[i].iov_base = iov_bufs[i];
		iovecs[i].iov_len = strlen(iov_bufs[i]);
	}
	
	ssize_t nwritten = writev(fd, iovecs, num_bufs);

	if (nwritten == -1) {
		char *err = "writev() failed";
		write(STDERR_FD, err, strlen(err));
		return 1;
	}
	
	return 0;
}

int main(int argc, char **argv)
{
	if (argc != 2) {
		usage(argv[0]);
		return 1;
	}

	char *write_file = argv[1];

	/* run the test(s) */
	int ret = writev_test(write_file);
	return ret;
}

