#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(int argc, char** argv) {
    char buffer[1024];
    if ((argc < 2) || (argc > 5)) {
        printf("[!] ERROR");
        exit(-1);
    }
    

    int ip = inet_addr(argv[1]);
    int puerto = atoi(argv[2]);

    FILE* fd = fopen("credenciales.txt", "rw");
    if (fd == NULL) {
        perror("[!] ERROR: no se ha podido abrir el fichero\n");
        exit(-1);
    }

    int sock1 = socket(AF_INET, SOCK_STREAM, 0);
    if (sock1 < 0) {
        perror("[!] ERROR: no se ha podido abrir el socket\n");
        exit(-1);
    }

    struct sockaddr_in servidor;
    servidor.sin_family = AF_INET;
    servidor.sin_port = htons(puerto);
    servidor.sin_addr.s_addr = ip;

    if (connect(sock1, (struct sockaddr*)&servidor, sizeof(servidor)) < 0) {
        perror("No se pudo conectar al servidor");
        exit(-1);
    }
    
    if (argc == 5) {
        // <ip> <puerto> <usuario> <secreto>
        char* usuario = argv[3];
        char* secreto = argv[4];
        printf("Autenticando con usuario %s\n", usuario);

        // Escribir en el fichero
        fprintf(fd, "%s %s", usuario, secreto);
    } else {
        // Leer fichero
        char usuario[100];
        char secreto[100];
        fscanf(fd, "%s %s", usuario, secreto);
        printf("Nombre y secreto encontrados\n");
        // printf("Autenticando con usuario %s\n", usuario);
    }

    // Enviar y recibir mensajes con el servidor, haciendo uso de streams
    FILE* f = fdopen(sock1, "r+");
    setbuf(f, NULL);

    while (1) {
        // Enviar mensaje al servidor
        char mensaje[100];
        fgets(mensaje, 100, stdin);
        fprintf(f, "%s", mensaje);
        fflush(f);

        
        // Recibir mensaje del servidor
        fgets(buffer, 1024, f);
        buffer[strcspn(buffer, "\n")] = '\0'; // Elimina el salto de línea si existe
        printf("%s\n", buffer);
        
        if (strcmp(buffer, "PROTO ERROR") == 0){ // Si el servidor responde con PROTO ERROR, se cierra la conexión
            exit(-1);
        }

    }
    return 0;
}
