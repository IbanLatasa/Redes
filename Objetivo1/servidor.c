#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>

int main(int argc, char* argv[]){

    // <servidor><puerto><userfile><nretos>
    if (argc!= 4) {
        printf("[+] Número invalido de argumentos!\n");
        exit(-1);
    }

    int puerto = atoi(argv[1]);
    int nretos = atoi(argv[3]);
    FILE *f = fopen(argv[2], "rw");
    if (f == NULL) {
        perror("[!] ERROR: no se ha podido abrir el fichero\n");
        exit(-1);
    }

    struct sockaddr_in servidor;
    servidor.sin_family = AF_INET;
    servidor.sin_port = htons(puerto);
    servidor.sin_addr.s_addr = INADDR_ANY; // Se conecta a cualquier dirección IP

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("[!] ERROR: no se ha podido abrir el socket\n");
        exit(-1);
    }
    
    if (connect(sock, (struct sockaddr*)&servidor, sizeof(servidor)) < 0) {
        perror("No se pudo conectar al servidor");
        exit(-1);
    }

    // if (bind(sock, (struct sockaddr *)&servidor, sizeof(servidor)) == -1) {
    //     perror ("\nERROR, no se pudo coger el puerto correctamente\n");
    //     exit(-1);
    // }

    // if(listen(sock, 5) == -1){ //Espera a una conexión
    //     perror("Error en listen");
    //     exit(-1);
    // }

    sleep(10);
}