
#define POLLIN 0x0001

typedef unsigned int nfds_t;
struct pollfd{
	int     fd;//       The following descriptor being polled. 
	short  events;//   The input event flags (see below). 
	short  revents;//  The output event flags (see below). 
};

#ifdef __cplusplus
extern "C" {
#endif

int poll(struct pollfd fds[], nfds_t nfds, int timeout);

#ifdef __cplusplus
}
#endif
