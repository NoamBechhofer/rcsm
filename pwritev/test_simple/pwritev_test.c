#include <sys/mman.h>
#include <sys/uio.h>

#include <fcntl.h>
#include <unistd.h>

#include <stdlib.h>
#include <string.h>

/**
 * I can't think of a situation where stderr wouldn't be 2, but in case it is,
 * this can be changed to the correct file descriptor for stderr.
 */
#define STDERR_FD 2
/**
 * I can't think of a situation where stdout wouldn't be 1, but in case it is,
 * this can be changed to the correct file descriptor for stdout.
 */
#define STDOUT_FD 1

/* reverse:  reverse string s in place
 * Adapted from K & R
 * Returns a pointer to @s
 */
char* reverse(char s[])
{
    int i, j;
    char c;

    for (i = 0, j = strlen(s) - 1; i < j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }

    return s;
}

/* itoa:  convert n to characters in s
 * Adapted from K & R
 * Returns a pointer to @s
 */
char* itoa(int n, char s[])
{
    int i, sign;

    if ((sign = n) < 0) /* record sign */
        n = -n; /* make n positive */
    i = 0;
    do { /* generate digits in reverse order */
        s[i++] = n % 10 + '0'; /* get next digit */
    } while ((n /= 10) > 0); /* delete it */
    if (sign < 0)
        s[i++] = '-';
    s[i] = '\0';
    return reverse(s);
}

void usage(char* prog_name)
{
    char* usage_msg[] = {
        "Usage: ",
        prog_name,
        " <write_file> <offset>\n"
    };
    size_t usage_msg_segments = 3;

    for (size_t i = 0; i < usage_msg_segments; i++) {
        char* segment = usage_msg[i];
        if (segment == NULL)
            continue;
        size_t segment_len = strlen(segment);
        ssize_t nwritten = 0;
        while (*segment != '\0') {
            nwritten = write(STDOUT_FD, segment, segment_len);
            segment_len -= nwritten;
            segment += nwritten;
        }
    }
}

/**
 * regular pwritev test
 * @return 0 if successful, non-zero otherwise
 */
int pwritev_test(char* write_file, size_t offset)
{
    char* iov_bufs[] = {
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
        char* err = "error on open()\n";
        write(STDERR_FD, err, strlen(err));
        return 1;
    }

    struct iovec iovecs[num_bufs];

    for (int i = 0; i < num_bufs; i++) {
        iovecs[i].iov_base = iov_bufs[i];
        iovecs[i].iov_len = strlen(iov_bufs[i]);
    }

    ssize_t nwritten = pwritev(fd, iovecs, num_bufs, offset);

    if (nwritten == -1) {
        char* err = "pwritev() failed";
        write(STDERR_FD, err, strlen(err));
        return 1;
    }

    // equiv. printf("wrote %d bytes\n", nwritten);
    /** buffer for itoa */
    char nwritten_str[12];
    char* success[] = {
        "wrote ",
        itoa(nwritten, nwritten_str),
        " bytes\n"
    };
    size_t success_segments = 3;
    for (size_t i = 0; i < success_segments; i++) {
        char* segment = success[i];
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

    return 0;
}

int main(int argc, char** argv)
{
    if (argc != 3) {
        usage(argv[0]);
        return 1;
    }

    char* write_file = argv[1];
    size_t offset = atoi(argv[2]);

    /* run the test(s) */
    int ret = pwritev_test(write_file, offset);
    return ret;
}
