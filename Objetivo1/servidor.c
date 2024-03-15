#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>


FILE *g;
int sock, sock_c;
void ctrl_c(int signal){
    printf("\n[!] Cerrando el servidor\n");
    //Cerrar sockets
    close(sock);
    close(sock_c);
    fclose(g);
    exit(-1);
}
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

    struct sockaddr_in servidor,cliente;
    servidor.sin_family = AF_INET;
    servidor.sin_port = htons(puerto);
    servidor.sin_addr.s_addr = INADDR_ANY; // Se conecta a cualquier dirección IP

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("[!] ERROR: no se ha podido abrir el socket\n");
        exit(-1);
    }

    // Asignamos el puerto al socket
    if (bind(sock, (struct sockaddr *)&servidor, sizeof(servidor)) == -1) {
        perror ("\nERROR, no se pudo coger el puerto correctamente\n");
        exit(-1);
    }

    if(listen(sock, 5) == -1){ //Espera a una conexión
        perror("Error en listen");
        exit(-1);
    }
    char buf[10000], cha[10000];
    while(1){
        signal(SIGINT, ctrl_c);
        int tam = sizeof(cliente);
        sock_c = accept(sock, (struct sockaddr *)&cliente, (socklen_t*)&tam);
        if (sock_c < 0) {
            perror("Error en accept");
            exit(-1);
        }
        g = fdopen(sock_c, "r+"); //Le doy permisos de lectura y escritura
        setbuf(g, NULL);
        // Recibir mensaje del cliente
        fgets(buf, 1024, g);
        printf("%s", buf);

        //Si el mensaje es "LOGIN <usuario>" entonces le respondo con "RETO <reto> <n>"
        if (strncmp(buf, "LOGIN", 5) == 0) {
            srand(time(NULL));
            int reto = rand() % nretos + 1;
            fprintf(g, "RETO %d %d\n", reto, nretos);
            
            //mientras siga recibiendo "RESPUESTA y un caracter" entonces le devuelvo otro reto
            fgets(cha, 1024, g);
            while (strncmp(cha, "RESPUESTA", 9) == 0) {
                reto = rand() % nretos + 1;
                fprintf(g, "RETO %d %d\n", reto, nretos);
                fgets(cha, 1024, g);
                nretos--;
                if (nretos == 0) {
                    fprintf(g, "LOGIN OK\n");
                    close(sock_c);
                    fclose(g);
                    exit(0);
                }

            }
        }
    }
    return 0;

}