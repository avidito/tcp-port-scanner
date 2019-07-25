#define MAX_SIZE 1000
#define BUFFER_SIZE 200
#define MAX_SERV 10

extern int terminate(const char *err, int sock, int debug);
extern int scanning_local(const int port, const int timeout, int debug);
extern int scanning(const char *host, const int port, const int timeout, int debug);
extern int sending(const char *host, int port, const char* message, int debug);
extern void itos(int num, char str[]);
extern char* get_element(char line[BUFFER_SIZE]);
extern int extract_xml(FILE *fp, char server_adress[BUFFER_SIZE], char ip_dst[MAX_SERV][BUFFER_SIZE], int port_dst[MAX_SERV], int *timeout);
extern int extract_xml_local(FILE *fp, char server_address[BUFFER_SIZE], char host[BUFFER_SIZE], int port[MAX_SERV], int *timeout);
extern void jsonstr(char *host, int port, int status, char message[BUFFER_SIZE], int debug);
