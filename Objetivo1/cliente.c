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
        char* nombre = argv[3];
        char* contrasena = argv[4];
        printf("Autenticando con usuario %s\n", nombre);
        // Escribir en el fichero
        fprintf(fd, "%s %s", nombre, contrasena);
    }

    char usuario[100];
    char secreto[100];

    fscanf(fd, "%s %s", usuario, secreto);
    
    // Enviar y recibir mensajes con el servidor, haciendo uso de streams
    FILE* f = fdopen(sock1, "r+");
    setbuf(f, NULL);

    // Enviar mensaje al servidor "LOGIN <usuario>"
    char mensaje[1024];
    sprintf(mensaje, "LOGIN %s\n", usuario);
    fprintf(f, "%s", mensaje);
    printf("%s", mensaje);
    fflush(f);


    // Recibir mensaje del servidor
    fgets(buffer, 1024, f);
    buffer[strcspn(buffer, "\n")] = '\0'; // Elimina el salto de línea si existe
    printf("%s\n", buffer);

    int reto = 1;
    while((strcmp(buffer, "LOGIN OK") != 0)){
        // Me guardo la respuesta del servidor
        char* token = strtok(buffer, " ");
        token = strtok(NULL, " ");
        int retos = atoi(token);
        token = strtok(NULL, " ");
        int intentos = atoi(token);

        // El usuario mandara el caracter del secreto que esta en la posicion, que es el reto mod longitud del secreto
        int posicion = retos % strlen(secreto);
        char caracter = secreto[posicion];
        sprintf(mensaje, "RESPUESTA %c\n", caracter);
        printf("reto %d respondido\n",reto);
        reto++;

        // Enviar mensaje al servidor
        fprintf(f, "%s", mensaje);
        fflush(f);

        // Recibir mensaje del servidor
        fgets(buffer, 1024, f);
        buffer[strcspn(buffer, "\n")] = '\0'; // Elimina el salto de línea si existe
        printf("%s\n", buffer);
        
        if (strcmp(buffer, "PROTO ERROR") == 0){ // Si el servidor responde con PROTO ERROR, se cierra la conexión
            exit(-1);
        }
        if (strcmp(buffer, "LOGIN OK") == 0){ // Si el servidor responde con LOGIN OK, se cierra la conexión
            printf("Autenticado OK\n");
        }

    }
    
    while(1){
        printf("1- consultar nombre\n");
        printf("2- cambiar nombre\n");
        printf("3- salir\n");
        int opcion;
        scanf("%d", &opcion);
        if (opcion == 1){
            sprintf(mensaje, "GETNAME\n");
            fprintf(f, "%s", mensaje);
            fflush(f);
            fgets(buffer, 1024, f);
            buffer[strcspn(buffer, "\n")] = '\0'; // Elimina el salto de línea si existe
            printf("%s\n", buffer);
            printf("\n");
        }
        if (opcion == 2){
            printf("Introduce el nuevo nombre: ");
            char nombre[100];
            scanf("%s", nombre);

            sprintf(mensaje, "SETNAME %s\n",nombre);
            fprintf(f, "%s", mensaje);
            fflush(f);
            fgets(buffer, 1024, f);
            buffer[strcspn(buffer, "\n")] = '\0'; // Elimina el salto de línea si existe
            printf("%s\n", buffer);
            printf("\n");
        }
        if (opcion == 3){
            exit(0);
        }
    }
    return 0;
}
