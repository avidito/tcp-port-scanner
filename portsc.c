// Library untuk melakukan PortScanning

#include <stdio.h>		  // perror(), FILE
#include <stdlib.h>	    // atoi()
#include <sys/fcntl.h>  // fcntl(), F_SETFL, O_NONBLOCK
#include <errno.h>      // errno, EWOULDBLOCK, EINPROGRESS
#include <arpa/inet.h>  // struct sockaddr_in, inet_addr(), htons(), socket(),
												// SOCK_STREAM, connect(), getsockopt(), SOL_SOCKET,
												// SO_ERROR, AF_INET
#include <unistd.h> 		// close(), write()
#include <string.h>     // strlen(), strtok(), strncpy(), strncat(), strcmp()
#include <netdb.h>			// struct servent, getservbyport()

#define MAX_SIZE 		1000
#define BUFFER_SIZE 200
#define MAX_SERV 		10

// Memberikan error yang sesuai
int terminate(const char *err, int sock, int debug){
	if(debug){
			perror(err);
	}

  if(sock) close(sock);
  return 99;
}

// Mengecek koneksi ke host:port dengan TCP Non-Blocking Socket
int scanning_local(const int port, const int timeout, int debug){
  // Mengatur alamat untuk Koneksi
  struct sockaddr_in serv_addr;
  serv_addr.sin_family      = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port        = htons(port);

  // Mengatur timeout untuk tiap port
  struct timeval tv;
  tv.tv_sec  = timeout;   // dalam detik
  tv.tv_usec = 0;         // dalam microdetik

  // Mengatur write descriptor
  fd_set write_fd;
  FD_ZERO(&write_fd);

  // Mengatur Error buffer
  int       so_error;
  socklen_t so_error_len;
  so_error_len = sizeof(so_error);

  // Inisiasi socket
  int sock;
  if((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1){
    return terminate("scanning socket() error... ", sock, debug);
  }

  // Mengatur socket menjadi unblocking
  fcntl(sock, F_SETFL, O_NONBLOCK);

  // Mengoneksi socket dengan host:port
  if((connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr))) == -1){ // Non-block socket selalu menghasilkan nilai connect == -1
			switch(errno){
        case EWOULDBLOCK:         // Nonblocking socket masih mencoba membangun koneksi (buffer penerima tidak cukup)
        case EINPROGRESS:         // Nonblocking socket masih mencoba membangun koneksi (menunggu)
                          break;
        default: return terminate("scanning connect() error... ", sock, debug); // Koneksi gagal
      }
  }

  // Mengatur write descriptor untuk pengecekan Writability
  FD_SET(sock, &write_fd);

  // Mengecek koneksi terhadap host:port
  if((select(sock+1, NULL, &write_fd, NULL, &tv)) == -1){
    return terminate("scanning select() error... ", sock, debug);
  }

  // Mengecek error yang didapat ketika melakukan koneksi
  if((getsockopt(sock, SOL_SOCKET, SO_ERROR, &so_error, &so_error_len)) < 0){
    return terminate("scanning getsockopt() error...\n", sock, debug);
  }
	//
	// if((getservbyport(htons(port), NULL)) == NULL){
	// 	return terminate("service unknown...\n", sock);
	// }

  // Menutup socket
  close(sock);

  // Mengembalikan status dari koneksi
  // so_error == 0 --> Status Active
  // so_error != 0 --> Status Inactive
  return (so_error == 0);
}

int scanning(const char *host, const int port, const int timeout, int debug){
  // Mengatur alamat untuk Koneksi
  struct sockaddr_in serv_addr;
  serv_addr.sin_family      = AF_INET;
  serv_addr.sin_addr.s_addr = inet_addr(host);
  serv_addr.sin_port        = htons(port);

  // Mengatur timeout untuk tiap port
  struct timeval tv;
  tv.tv_sec  = timeout;   // dalam detik
  tv.tv_usec = 0;         // dalam microdetik

  // Mengatur write descriptor
  fd_set write_fd;
  FD_ZERO(&write_fd);

  // Mengatur Error buffer
  int       so_error;
  socklen_t so_error_len;
  so_error_len = sizeof(so_error);

  // Inisiasi socket
  int sock;
  if((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1){
    return terminate("scanning socket() error... ", sock, debug);
  }

  // Mengatur socket menjadi unblocking
  fcntl(sock, F_SETFL, O_NONBLOCK);

  // Mengoneksi socket dengan host:port
  if((connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr))) == -1){ // Non-block socket selalu menghasilkan nilai connect == -1
			switch(errno){
        case EWOULDBLOCK:         // Nonblocking socket masih mencoba membangun koneksi (buffer penerima tidak cukup)
        case EINPROGRESS:         // Nonblocking socket masih mencoba membangun koneksi (menunggu)
                          break;
        default: return terminate("scanning connect() error... ", sock, debug); // Koneksi gagal
      }
  }

  // Mengatur write descriptor untuk pengecekan Writability
  FD_SET(sock, &write_fd);

  // Mengecek koneksi terhadap host:port
  if((select(sock+1, NULL, &write_fd, NULL, &tv)) == -1){
    return terminate("scanning select() error... ", sock, debug);
  }

  // Mengecek error yang didapat ketika melakukan koneksi
  if((getsockopt(sock, SOL_SOCKET, SO_ERROR, &so_error, &so_error_len)) < 0){
    return terminate("scanning getsockopt() error...\n", sock, debug);
  }
	//
	// if((getservbyport(htons(port), NULL)) == NULL){
	// 	return terminate("service unknown...\n", sock);
	// }

  // Menutup socket
  close(sock);

  // Mengembalikan status dari koneksi
  // so_error == 0 --> Status Active
  // so_error != 0 --> Status Inactive
  return (so_error == 0);
}

// Fungsi untuk mengirimkan data ke server
int sending(const char *host, int port, const char* message, int debug){
  // Mengatur alamat pengiriman
  struct sockaddr_in serv_addr;
  serv_addr.sin_family      = AF_INET;
  serv_addr.sin_addr.s_addr = inet_addr(host);
  serv_addr.sin_port        = htons(port);

  // Membuat socket
  int sock;
  if((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1){
    return terminate("sending socket() error... ", sock, debug);
  }

  // Memulai koneksi dengan server
  if((connect(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr))) == -1){
    return terminate("sending connect() error... ", sock, debug);
  }

  // Mengirimkan data ke server
  if(write(sock, message, strlen(message)) == -1){
    return terminate("sending write() error... ", sock, debug);
  }

  // Menutup socket
  close(sock);

  // Mengembalikan kode pengiriman berhasil
  return 1;
}

// Fungsi untuk merubah Int menjadi String
void itos(int num, char str[]){
	int i 	= 0;
	int p	= 0;
	char con[10];
	do{
		con[i] = (num%10)+'0';
		num   /= 10;
		i++;
	}while(num);
	i--;
	while(p <= i){
		str[p] = con[i-p];
		p++;
	}
	str[i+1] = '\0';
}

// Fungsi untuk mengambil nilal antar tag
char* get_element(char line[BUFFER_SIZE]){
  int  i = 0;
  int  p;
  char *tmp;
  tmp = strtok(line, ">");
  tmp = strtok(NULL, "<");
  return tmp;
}

// Fungsi untuk mengekstrak informasi dari file XML
int extract_xml(FILE *fp, char server_address[BUFFER_SIZE], char ip_dst[MAX_SERV][BUFFER_SIZE], int port_dst[MAX_SERV], int *timeout){
  int  i, p, c;
  char str[BUFFER_SIZE] = "\0";
  char buffer[BUFFER_SIZE];

  // Mengambil <root>
  fgets(buffer, BUFFER_SIZE, fp);

  // Mengambil alamat tujuan
	fgets(buffer, BUFFER_SIZE, fp);
  strncpy(server_address, get_element(buffer), BUFFER_SIZE);

  // Mengambil waktu timeout
  fgets(buffer, BUFFER_SIZE, fp);
  *timeout = atoi(get_element(buffer));

  // Mengambil <service>
  fgets(buffer, BUFFER_SIZE, fp);

  c = 0;
  while(strcmp(buffer, "  <service>\n") == 0){
      // Mengambil IP
      fgets(buffer, BUFFER_SIZE, fp);
      strncpy(ip_dst[c], get_element(buffer), BUFFER_SIZE);

      // Mengambil port
      fgets(buffer, BUFFER_SIZE, fp);
      port_dst[c] = atoi(get_element(buffer));

      // Mengambil </service>
      fgets(buffer,BUFFER_SIZE, fp);

      // Mengambil baris selanjutnya untuk pengecekan
      fgets(buffer, BUFFER_SIZE, fp);
      c++;
  }
	return c;
}

// Fungsi untuk mengekstrak informasi dari file XML
int extract_xml_local(FILE *fp, char server_address[BUFFER_SIZE], char host[BUFFER_SIZE], int port[MAX_SERV], int *timeout){
  int  i, p, c;
  char str[BUFFER_SIZE] = "\0";
  char buffer[BUFFER_SIZE];

  // Mengambil <root>
  fgets(buffer, BUFFER_SIZE, fp);

  // Mengambil alamat agent
  fgets(buffer, BUFFER_SIZE, fp);

  strncpy(host, get_element(buffer), BUFFER_SIZE);

  // Mengambil alamat tujuan
	fgets(buffer, BUFFER_SIZE, fp);

  strncpy(server_address, get_element(buffer), BUFFER_SIZE);

  // Mengambil waktu timeout
  fgets(buffer, BUFFER_SIZE, fp);
  *timeout = atoi(get_element(buffer));

  // Mengambil port
  fgets(buffer, BUFFER_SIZE, fp);

  c = 0;
  while(strcmp(buffer, "</root>\n")){
      // Mengambil Port
      port[c] = atoi(get_element(buffer));

      // Mengambil baris selanjutnya untuk pengecekan
      fgets(buffer, BUFFER_SIZE, fp);
      c++;
  }
	return c;
}


void jsonstr(char host[], int port, int status, char message[BUFFER_SIZE], int debug){
  char buffer[BUFFER_SIZE] = "\0";
  // Memasukan data host
  strncat(message, "{\"host\":\"", 12);
  strncat(message, host, strlen(host));

  // Memasukan data port
  strncat(message, "\",\"port\":\"", 12);
  itos(port, buffer);
  strncat(message, buffer, strlen(buffer));

  // Memasukan data status
  strncat(message, "\",\"status\":\"", 12);
  itos(status, buffer);
  strncat(message, buffer, strlen(buffer));
  strncat(message, "\"}", 5);

	if(debug){
		printf("Created : %s\n", message);
	}
}
