/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 * Server. Prodedures registration and start
 */
#include <unistd.h>

#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <net/if.h>
#include <arpa/inet.h>

#include "msg.h"
#include <stdio.h>
#include <stdlib.h>
#include <rpc/pmap_clnt.h>
#include <string.h>
#include <memory.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <syslog.h>

#define DAEMON_NAME "rpc_server_daemon"

#ifndef SIG_PF
#define SIG_PF void(*)(int)
#endif

static void
messageprog_1(struct svc_req *rqstp, register SVCXPRT *transp)
{
	union {
		char *printmessage_1_arg;
	} argument;
	char *result;
	xdrproc_t _xdr_argument, _xdr_result;
	char *(*local)(char *, struct svc_req *);

	switch (rqstp->rq_proc) {
	case NULLPROC:
		(void) svc_sendreply (transp, (xdrproc_t) xdr_void, (char *)NULL);
		return;

	case PRINTMESSAGE:
		_xdr_argument = (xdrproc_t) xdr_wrapstring;
		_xdr_result = (xdrproc_t) xdr_int;
		local = (char *(*)(char *, struct svc_req *)) printmessage_1_svc;
		break;

	default:
		svcerr_noproc (transp);
		return;
	}
	memset ((char *)&argument, 0, sizeof (argument));
	if (!svc_getargs (transp, (xdrproc_t) _xdr_argument, (caddr_t) &argument)) {
		svcerr_decode (transp);
		return;
	}
	result = (*local)((char *)&argument, rqstp);
	if (result != NULL && !svc_sendreply(transp, (xdrproc_t) _xdr_result, result)) {
		svcerr_systemerr (transp);
	}
	if (!svc_freeargs (transp, (xdrproc_t) _xdr_argument, (caddr_t) &argument)) {
		fprintf (stderr, "%s", "unable to free arguments");
		exit (1);
	}
	return;
}

int
main (int argc, char **argv)
{
	register SVCXPRT *transp;
	if(argc < 2) {
		fprintf(stderr, "Check server name. Usage: ./server [servername]\n");
		exit(1);
	}
	char *server;
	server = argv[1];
	//get local ip adress on eth0
	int fd;
 	struct ifreq ifr;
	fd = socket(AF_INET, SOCK_DGRAM, 0);

	ifr.ifr_addr.sa_family = AF_INET;
 	strncpy(ifr.ifr_name, "eth0", IFNAMSIZ-1);
	ioctl(fd, SIOCGIFADDR, &ifr);
	close(fd);

	//fork off the parent process
	pid_t pid, sid;
	pid = fork();
	if(pid < 0) {
		exit(EXIT_FAILURE);
	}
	if(pid > 0) {
		exit(EXIT_SUCCESS);
	}

	umask(0);
	openlog("rpc_client_server", 0, LOG_USER);
	syslog(LOG_INFO, "server %s on ip %s started", server, inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
	pmap_unset (MESSAGEPROG, PRINTMESSAGETOCONSOLE);

	if((chdir("/")) < 0) {
		exit(EXIT_FAILURE);
	}
	transp = svcudp_create(RPC_ANYSOCK);
	if (transp == NULL) {
		fprintf (stderr, "%s", "cannot create udp service.");
		exit(1);
	}
	if (!svc_register(transp, MESSAGEPROG, PRINTMESSAGETOCONSOLE, messageprog_1, IPPROTO_UDP)) {
		fprintf (stderr, "%s", "unable to register (MESSAGEPROG, PRINTMESSAGETOCONSOLE, udp).");
		exit(1);
	}

	transp = svctcp_create(RPC_ANYSOCK, 0, 0);
	if (transp == NULL) {
		fprintf (stderr, "%s", "cannot create tcp service.");
		exit(1);
	}
	if (!svc_register(transp, MESSAGEPROG, PRINTMESSAGETOCONSOLE, messageprog_1, IPPROTO_TCP)) {
		fprintf (stderr, "%s", "unable to register (MESSAGEPROG, PRINTMESSAGETOCONSOLE, tcp).");
		exit(1);
	}
	while(1) {
		svc_run ();
		fprintf (stderr, "%s", "svc_run returned");
		exit (1);
	}
	/* NOTREACHED */
}