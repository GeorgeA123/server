enum requestMethod {
	HEAD,
	GET,
	UNSUPPORTED
}equestMethod;

typedef struct{
	enum requestMethod method;
	char* resource;
	char* version;
	char* userAgent;
	int status;
}httpHeader;
int service_client_socket (const int s, const char *const address);
