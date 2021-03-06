/*
	REMOTE VERSION OF FUNCTION
 */
#include <stdio.h>
#include <syslog.h>
#include <string.h>
#include "functions.h"			/* functions.h generated by rpcgen */
 
main(argc, argv)
	int argc;
	char *argv[];
{
	CLIENT *clnt;
	int *result;
	char *server;
	char *message;
	char *service;
	char *protocol;
 
	if (argc != 5) {
		fprintf(stderr, "usage: %s [ip] [service_1/service_2/service_3] [message] [udp/tcp]\n", argv[0]);
		exit(1);
	}
 
	server = argv[1];
	service = argv[2];
	message = argv[3];
	protocol = argv[4];
	
	
	openlog("rpc_client_server", 0, LOG_USER);
	syslog(LOG_INFO, "connection to server [%s]", server);
	syslog(LOG_INFO, "client message is [%s]", message);
	syslog(LOG_INFO, "client service is [%s]", service);


	if(strcmp(service, "service_1") == 0) {
		clnt = clnt_create(server, SERVICE_1, PRINTMSG_1, protocol);
		if (clnt == (CLIENT *)NULL) {
			syslog(LOG_INFO, "could't establish connection with server %s, aborting", server);
			clnt_pcreateerror(server);
			exit(1);
		}
		result = printmsg_1_1(&message, clnt);
		clnt_destroy(clnt);
	}
	else if(strcmp(service, "service_2") == 0) {
		clnt = clnt_create(server, SERVICE_2, PRINTMSG_2, protocol);
		if (clnt == (CLIENT *)NULL) {
			syslog(LOG_INFO, "could't establish connection with server %s, aborting", server);
			clnt_pcreateerror(server);
			exit(1);
		}
		result = printmsg_2_1(&message, clnt);
		clnt_destroy(clnt);	
	}
	else if(strcmp(service, "service_3") == 0) {
		clnt = clnt_create(server, SERVICE_3, PRINTMSG_3, protocol);
		if (clnt == (CLIENT *)NULL) {
			syslog(LOG_INFO, "could't establish connection with server %s, aborting", server);
			clnt_pcreateerror(server);
			exit(1);
		}	
		result = printmsg_2_1(&message, clnt);
		clnt_destroy(clnt);
	}
	//remote procedure call
	if (result == (int *)NULL) {
		syslog(LOG_INFO, "an error occured while calling the server, aborting");
		clnt_perror(clnt, server);
		exit(1);
	}
	if (*result == 0) {
		fprintf(stderr,"%s: could not print your message\n",argv[0]);
		exit(1);
	}
	syslog(LOG_INFO, "service [%s] at ip [%s] successfully delievered message [%s]", service, server, message);
	

	exit(0);
	
}