#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> 
#include <windows.h> 
#include <time.h>
#define MAX_BUFFER 2048

// Estructuras
struct StringNode {
    char* data;           // El contenido (Tweet o Nombre de usuario seguido)
    char* fecha;          // NUEVO: Fecha de publicación (o NULL si es un seguidor)
    struct StringNode* sig;
};
typedef struct StringNode StringNode;

// Estructura auxiliar solo para mostrar el Timeline ordenado
struct TweetDisplay {
    char autor[50];
    StringNode* tweet;
};
typedef struct TweetDisplay TweetDisplay;

struct User {
    char username[50];
    char password[50];
    StringNode* tweets;    
	 // Cabeza de la lista de tweets
    StringNode* following; 
	 // Cabeza de la lista de 'siguiendo'
    struct User* sig;      
	 // Siguiente usuario en la lista principal
};
typedef struct User User;

// Funciones de utilidad de listas
StringNode* createStringNode(const char* data, const char* fecha);
void addStringNode(StringNode** head, const char* data, const char* fecha);
User* createUser(const char* user, const char* pass);
void addUserToList(User** head, User* nvoUser);
User* findUser(User* head, const char* username);
int isFollowing(User* currentUser, const char* username);
int compararFechas(const char* f1, const char* f2);

// Funciones de carga y guardado de archivos
void guardarDatos(User* head);
User* cargarDatos();

// Funciones de limpieza de memoria
void liberarStringList(StringNode* head);
void liberarUserList(User* head);

// Funciones de Menús
void menuBienvenida(User** userList);
User* menuLogin(User* userList);
User* menuCrearUsuario(User** userList);
void menuTwitter(User* currentUser, User* userList);

// Funciones de Acciones de Twitter
void publicarTweet(User* currentUser, User* userList);
void verTweets(User* currentUser);
void borrarTweet(User* currentUser, User* userList);
void seguirAlguien(User* currentUser, User* userList);
void verSeguidores(User* currentUser, User* userList);
void verTimeline(User* currentUser, User* userList);

// Funciones Auxiliares
void limpiarPantalla();
void leerEntrada(char* buffer, int size);
char getOpcion();
void presionaXParaVolver();

// --- Método Main ---
int main() {
    SetConsoleCP(1252);
    SetConsoleOutputCP(1252);
    
    User* userList = cargarDatos(); 
	// Carga todos los datos de los .txt a la memoria (listas)
    // Se inicia el menú principal
    menuBienvenida(&userList); 
	// Recibe de parámetro la dirección para poder añadir usuarios
    // Libera toda la memoria asignada
    liberarUserList(userList);
    
    printf("Gracias por usar Twitter. ¡Adiós!\n");
    return 0;
}

StringNode* createStringNode(const char* data, const char* fecha) {
    StringNode* nvo = (StringNode*)malloc(sizeof(StringNode));
    
    // 1. Asignar memoria y copiar el dato principal (Tweet/Nombre)
    nvo->data = (char*)malloc(strlen(data) + 1);
    strcpy(nvo->data, data);
    
    // 2. Asignar memoria para la fecha SOLO si se proporcionó una
    if (fecha != NULL) {
        nvo->fecha = (char*)malloc(strlen(fecha) + 1);
        strcpy(nvo->fecha, fecha);
    } else {
        nvo->fecha = NULL; // Importante para saber que no tiene fecha
    }

    nvo->sig = NULL;
    return nvo;
}

// Inserta un nodo string final de la lista
void addStringNode(StringNode** head, const char* data, const char* fecha) {
    StringNode* nvo = createStringNode(data, fecha);
    if (*head == NULL) {
        *head = nvo;
    } else {
        StringNode* p = *head;
        while (p->sig != NULL) {
            p = p->sig;
        }
        p->sig = nvo;
    }
}

User* createUser(const char* user, const char* pass) {
    User* nvo = (User*)malloc(sizeof(User));
    strcpy(nvo->username, user);
    strcpy(nvo->password, pass);
    nvo->tweets = NULL;
    nvo->following = NULL;
    nvo->sig = NULL;
    return nvo;
}

// Inserta al final de la lista de usuarios
void addUserToList(User** head, User* nvoUser) {
    if (*head == NULL) {
        *head = nvoUser;
    } else {
        User* p = *head;
        while (p->sig != NULL) {
            p = p->sig;
        }
        p->sig = nvoUser;
    }
}

User* findUser(User* head, const char* username) {
    User* p = head;
    while (p != NULL) {
        if (strcmp(p->username, username) == 0) {
            return p; // Encontrado
        }
        p = p->sig;
    }
    return NULL; // No encontrado
}

int isFollowing(User* currentUser, const char* username) {
    StringNode* p = currentUser->following;
    while (p != NULL) {
        if (strcmp(p->data, username) == 0) {
            return 1; // Verdadero, ya lo sigue
        }
        p = p->sig;
    }
    return 0; // Falso
}

void liberarStringList(StringNode* head) {
    StringNode* aux;
    while (head != NULL) {
        aux = head;
        head = head->sig;
        free(aux->data);
        if (aux->fecha != NULL) free(aux->fecha); // <--- Liberar fecha
        free(aux);
    }
}

// Retorna 1 si f1 es más reciente que f2, -1 si es más antiguo, 0 si son iguales
int compararFechas(const char* f1, const char* f2) {
    int d1, m1, a1, h1, min1;
    int d2, m2, a2, h2, min2;

    // Si alguno no tiene fecha, lo tratamos como muy antiguo
    if (f1 == NULL) return -1;
    if (f2 == NULL) return 1;

    // Extraemos los numeros del string: DD/MM/AAAA HH:MM
    sscanf(f1, "%d/%d/%d %d:%d", &d1, &m1, &a1, &h1, &min1);
    sscanf(f2, "%d/%d/%d %d:%d", &d2, &m2, &a2, &h2, &min2);

    // Jerarquía de comparación: Año -> Mes -> Día -> Hora -> Minuto
    if (a1 > a2) return 1;
    if (a1 < a2) return -1;
    
    if (m1 > m2) return 1;
    if (m1 < m2) return -1;
    
    if (d1 > d2) return 1;
    if (d1 < d2) return -1;
    
    if (h1 > h2) return 1;
    if (h1 < h2) return -1;
    
    if (min1 > min2) return 1;
    if (min1 < min2) return -1;

    return 0; // Son exactamente iguales
}

void liberarUserList(User* head) {
    User* aux;
    while (head != NULL) {
        aux = head;
        head = head->sig;
        // Libera las listas anidadas
        liberarStringList(aux->tweets);
        liberarStringList(aux->following);
        // Libera el usuario
        free(aux);
    }
}

//Menús---
void menuBienvenida(User** userList) {
    char opcion;
    do {
        limpiarPantalla();
        printf("-------------------------\n");
        printf("  BIENVENIDO A TWITTER\n");
        printf("-------------------------\n");
        printf(" 1) Crear Usuario\n");
        printf(" 2) Registrarse\n");
        printf(" X) Salir\n");
        printf("Opcion: ");
        opcion = getOpcion();

        User* usuarioLogueado = NULL;

        switch (opcion) {
            case '1':
                usuarioLogueado = menuCrearUsuario(userList);
                if (usuarioLogueado != NULL) {
                    menuTwitter(usuarioLogueado, *userList);
                }
                break;
            case '2':
                usuarioLogueado = menuLogin(*userList);
                if (usuarioLogueado != NULL) {
                    menuTwitter(usuarioLogueado, *userList);
                }
                break;
            case 'x':
                break; // Sale del bucle
            default:
                printf("Opcion no valida...\n");
                Sleep(1000);
        }
    } while (opcion != 'x');
}

User* menuCrearUsuario(User** userList) {
    char nvoUser[50], nvoPass[50];
    
    limpiarPantalla();
    printf("-------------------------\n");
    printf(" Crear Nuevo Usuario \n");
    printf("-------------------------\n");
    printf("Ingresa tu nombre de usuario: ");
    leerEntrada(nvoUser, 50);

    // Verifica si el usuario ya existe
    if (findUser(*userList, nvoUser) != NULL) {
        printf("Error: El usuario '%s' ya existe.\n", nvoUser);
        Sleep(2000);
        return NULL;
    }

    printf("Crea una contraseña: ");
    leerEntrada(nvoPass, 50);

    //Crea el usuario en memoria
    User* nvo = createUser(nvoUser, nvoPass);
    addUserToList(userList, nvo);
    
    //Guarda todos los datos actualizados en los archivos
    guardarDatos(*userList);
    printf("-------------------------\n");
    printf("¡Usuario '%s' creado con exito!\n", nvoUser);
    printf("-------------------------\n");
    Sleep(1500);
    // Devuelve el nuevo usuario para iniciar sesión
    return nvo;
}

User* menuLogin(User* userList) {
    char username[50], password[50];
    
    while (1) { // Bucle del login
        limpiarPantalla();
        printf("-------------------------\n");
        printf("--- Iniciar Sesion ---\n");
        printf("-------------------------\n");
        printf("Usuario: ");
        leerEntrada(username, 50);

        User* usuarioEncontrado = findUser(userList, username);

        if (usuarioEncontrado == NULL) {
            // Caso 1: Usuario no encontrado
            printf("Usuario no encontrado.\n");
            printf("-------------------------\n");
            printf("¿Deseas crear uno nuevo?\n");
            printf(" 1) SI\n 2) NO (Reintentar)\n X) Salir\n");
            printf("Opcion: ");
            char opcion = getOpcion();
            
            if (opcion == '1') {
                return menuCrearUsuario(&userList); // Llama a crear usuario
            } else if (opcion == 'x') {
                return NULL; // Regresa al menú principal
            }
            // Si es '2' o cualquier otra cosa, el bucle se repite
        
        } else {
            // Caso 2: Usuario encontrado, pedir contraseña
            while (1) { // Bucle de contraseña
                printf("Contrasena: ");
                leerEntrada(password, 50);

                if (strcmp(usuarioEncontrado->password, password) == 0) {
                    // Contraseña correcta
                    printf("¡Bienvenido, %s!\n", usuarioEncontrado->username);
                    Sleep(1500);
                    return usuarioEncontrado;
                } else {
                    // Contraseña incorrecta
                    printf("-------------------------\n");
                    printf("Contrasena Incorrecta.\n");
                    printf("¿Que deseas hacer?\n");
                    printf(" 1) VOLVER A ESCRIBIR\n X) Salir\n");
                    printf("Opcion: ");
                    char opcion = getOpcion();
                    
                    if (opcion == 'x') {
                        return NULL; // Regresa al menú principal
                    }
        // Si es '1' o cualquier otra cosa, el bucle de contraseña repite
                }
            }
        }
    }
}

void menuTwitter(User* currentUser, User* userList) {
    char opcion;
    do {
        limpiarPantalla();
        printf("---------------------------------\n");
        printf("Hola, %s. ¿Que deseas hacer?\n", currentUser->username);
        printf("---------------------------------\n");
        printf(" 1) PUBLICAR UN TWEET\n");
        printf(" 2) VER MIS TWEETS\n");
        printf(" 3) SEGUIR A ALGUIEN\n");
        printf(" 4) VER A MIS SEGUIDORES\n");
        printf(" 5) VER TWEETS\n");
        printf(" 6) BORRAR UN TWEET\n");
        printf(" X) SALIR (Cerrar Sesion)\n");
        printf("Opcion: ");
        opcion = getOpcion();

        switch (opcion) {
            case '1':
                publicarTweet(currentUser, userList);
                break;
            case '2':
                verTweets(currentUser);
                break;
            case '3':
                seguirAlguien(currentUser, userList);
                break;
            case '4':
                verSeguidores(currentUser, userList);
                break; 
			case '5':
                verTimeline(currentUser, userList);
                break;
			case '6':
                borrarTweet(currentUser, userList);
                break;	   
            case 'x':
                break; // Sale al menú principal
            default:
                printf("Opcion no valida...\n");
                Sleep(1000);
        }
    } while (opcion != 'x');
}

//Acciones de Twitter---

void publicarTweet(User* currentUser, User* userList) {
    char tweet[280]; 
    char fechaActual[50]; // Buffer para la fecha
    
    limpiarPantalla();
    printf("Escribe tu tweet (max 280 caracteres):\n");
    leerEntrada(tweet, 280);

    if (strlen(tweet) == 0) {
        printf("No se puede publicar un tweet vacio.\n");
    } else {
        // --- OBTENER FECHA Y HORA ---
        time_t t = time(NULL);
        struct tm *tm = localtime(&t);
        // Formato: DD/MM/AAAA HH:MM
        strftime(fechaActual, sizeof(fechaActual), "%d/%m/%Y %H:%M", tm);

        // Añade el tweet a la lista CON FECHA
        addStringNode(&(currentUser->tweets), tweet, fechaActual);
        
        guardarDatos(userList);
        
        printf("\nTWEET PUBLICADO a las %s\n", fechaActual);
    }
    
    Sleep(2000);
}

void verTweets(User* currentUser) {
    limpiarPantalla();
    printf("--- MIS TWEETS (%s) ---\n", currentUser->username);
    
    StringNode* tweet = currentUser->tweets;
    int i = 1;
    
    if (tweet == NULL) {
        printf("Aun no has publicado nada.\n");
    } else {
        while (tweet != NULL) {
            printf("\nTweet %d (%s):\n", i, (tweet->fecha ? tweet->fecha : "N/A"));
            printf(" %s\n", tweet->data);
            i++;
            tweet = tweet->sig;
        }
    }
    
    presionaXParaVolver();
}

void seguirAlguien(User* currentUser, User* userList) {
    limpiarPantalla();
    printf("¿A quien vas a seguir?\n");

    User* p = userList;
    User* usuariosDisponibles[100]; 
	// Límite de 100 usuarios para mostrar
    int count = 0;

    // 1. Mostrar todos los usuarios EXCEPTO el actual
    while (p != NULL && count < 100) {
        if (strcmp(p->username, currentUser->username) != 0) {
            printf(" %d) %s\n", (count + 1), p->username);
            usuariosDisponibles[count] = p;
            count++;
        }
        p = p->sig;
    }

    if (count == 0) {
        printf("No hay otros usuarios para seguir.\n");
        presionaXParaVolver();
        return;
    }

    printf(" X) Volver\n");
    printf("Elige un numero: ");
    
    char buffer[10];
    int opcion;
    leerEntrada(buffer, 10);
    
    if (tolower(buffer[0]) == 'x') return;
    
    if (sscanf(buffer, "%d", &opcion) != 1 || opcion < 1 || opcion > count) {
        printf("Opcion no valida.\n");
        Sleep(1000);
        return;
    }
    
    User* usuarioASeleccionar = usuariosDisponibles[opcion - 1];

    //Verifica si ya lo sigue
    if (isFollowing(currentUser, usuarioASeleccionar->username)) {
        printf("Ya estas siguiendo a %s.\n", usuarioASeleccionar->username);
    } else {
        //Añade a la lista en memoria
        addStringNode(&(currentUser->following), usuarioASeleccionar->username, NULL);
        //Guarda todo en los archivos
        guardarDatos(userList);
        
        printf("Ahora estas siguiendo a %s.\n", usuarioASeleccionar->username);
    }
    
    Sleep(1000);
        return;
}

void verSeguidores(User* currentUser, User* userList) {
    limpiarPantalla();
    printf("--- LISTA DE SEGUIDORES DE %s ---\n", currentUser->username);
	printf("------------------------------------\n");
    User* p = userList; // Puntero para recorrer toda la lista de usuarios
    int contadorSeguidores = 0;
    //Recorrido de toda la lista de usuarios
    while (p != NULL) {
        //Nos comprobamos a nosotros mismos
        if (strcmp(p->username, currentUser->username) != 0) {
            // Se revisa que ¿Este usuario 'p' sigue a 'currentUser'?
            if (isFollowing(p, currentUser->username)) {
                //Si es así, se imprimir'a
                printf("- %s\n", p->username);
                contadorSeguidores++;
            }
        }
        
        p = p->sig; // Pasamos al siguiente usuario de la red
    }

    if (contadorSeguidores == 0) {
        printf("\nAun no tienes seguidores.\n");
    }
    presionaXParaVolver();
}

void verTimeline(User* currentUser, User* userList) {
    // Bucle para mantenernos en el Timeline hasta que queramos salir
    while (1) {
        limpiarPantalla();
        printf("---------------------------\n");
        printf("         TIMELINE \n");
        printf("   (Mas recientes primero)\n");
        printf("---------------------------\n");

        // 1. RECOLECCIÓN
        TweetDisplay feed[500]; 
        int totalTweets = 0;
        
        StringNode* pFollow = currentUser->following;

        while (pFollow != NULL) {
            User* amigo = findUser(userList, pFollow->data);
            if (amigo != NULL) {
                StringNode* pTweet = amigo->tweets;
                while (pTweet != NULL && totalTweets < 500) {
                    strcpy(feed[totalTweets].autor, amigo->username);
                    feed[totalTweets].tweet = pTweet;
                    totalTweets++;
                    pTweet = pTweet->sig;
                }
            }
            pFollow = pFollow->sig;
        }

        if (totalTweets == 0) {
            printf("No hay tweets para mostrar.\n");
            presionaXParaVolver();
            return;
        }

        // 2. ORDENAMIENTO (Burbuja)
        int i, j;
        for (i = 0; i < totalTweets - 1; i++) {
            for (j = 0; j < totalTweets - i - 1; j++) {
                if (compararFechas(feed[j].tweet->fecha, feed[j+1].tweet->fecha) < 0) {
                    TweetDisplay temp = feed[j];
                    feed[j] = feed[j+1];
                    feed[j+1] = temp;
                }
            }
        }

        // 3. IMPRESIÓN CON NÚMEROS
        for (i = 0; i < totalTweets; i++) {
            StringNode* t = feed[i].tweet;
            // Imprimimos un número índice (i+1) para poder seleccionar
            printf("%d) @%s [%s]:\n", i + 1, feed[i].autor, (t->fecha ? t->fecha : "S/F"));
            printf("   %s\n", t->data);
            printf("---------------------------\n");
        }

        // 4. MENÚ INTERACTIVO
        printf("\n[R]etwittear un tweet  |  [X] Volver al menu\n");
        printf("Elige una opcion: ");
        
        char buffer[10];
        leerEntrada(buffer, 10);
        char opcion = tolower(buffer[0]);

        if (opcion == 'x') {
            break; // Sale de la función
        } 
        else if (opcion == 'r') {
            printf("Ingresa el numero del tweet a Retwittear: ");
            int numTweet;
            leerEntrada(buffer, 10);
            
            // Validar la entrada
            if (sscanf(buffer, "%d", &numTweet) == 1 && numTweet >= 1 && numTweet <= totalTweets) {
                
                // --- LÓGICA DE RETWEET ---
                TweetDisplay seleccionado = feed[numTweet - 1];
                char nuevoTexto[350]; // Espacio extra para "RT @..."
                char fechaActual[50];

                // Formateamos el nuevo tweet: "RT @Autor: Texto"
                // snprintf es una version segura de sprintf para no desbordar el buffer
                snprintf(nuevoTexto, sizeof(nuevoTexto), "RT @%s: %s", 
                         seleccionado.autor, seleccionado.tweet->data);

                // Obtenemos fecha actual
                time_t t = time(NULL);
                struct tm *tm = localtime(&t);
                strftime(fechaActual, sizeof(fechaActual), "%d/%m/%Y %H:%M", tm);

                // Guardamos
                addStringNode(&(currentUser->tweets), nuevoTexto, fechaActual);
                guardarDatos(userList);

                printf("\n¡Has retwitteado a @%s con exito!\n", seleccionado.autor);
                Sleep(1500);
                // El bucle while(1) se repetirá y veremos nuestro RT arriba del todo
            } else {
                printf("Numero no valido.\n");
                Sleep(1000);
            }
        }
    }
}

//Carga y Guardado ---
void guardarDatos(User* head) {
    FILE *f_users, *f_tweets, *f_follows;

    f_users = fopen("usuarios.txt", "w");
    f_tweets = fopen("usuarios_tweet.txt", "w");
    f_follows = fopen("usuarios_follows.txt", "w");

    if (f_users == NULL || f_tweets == NULL || f_follows == NULL) {
        printf("ERROR FATAL: No se pudieron abrir los archivos para guardar.\n");
        exit(1);
    }

    User* pUser = head;
    while (pUser != NULL) {
        // Guarda en usuarios.txt (Mantenemos la coma aquí, ya que usuario/pass no suelen tener espacios ni comas)
        fprintf(f_users, "%s,%s\n", pUser->username, pUser->password);

        // Guarda en usuarios_tweet.txt USANDO PIPE '|'
        fprintf(f_tweets, "%s", pUser->username);
        StringNode* pTweet = pUser->tweets;
        while (pTweet != NULL) {
            char* fechaStr = (pTweet->fecha != NULL) ? pTweet->fecha : "N/A";
            fprintf(f_tweets, "|%s|%s", fechaStr, pTweet->data);
            pTweet = pTweet->sig;
        }
        fprintf(f_tweets, "\n");

        // Guarda en usuarios_follows.txt USANDO PIPE '|'
        fprintf(f_follows, "%s", pUser->username);
        StringNode* pFollow = pUser->following;
        while (pFollow != NULL) {
            fprintf(f_follows, "|%s", pFollow->data); // <--- CAMBIO AQUÍ
            pFollow = pFollow->sig;
        }
        fprintf(f_follows, "\n");

        pUser = pUser->sig;
    }

    fclose(f_users);
    fclose(f_tweets);
    fclose(f_follows);
}

void borrarTweet(User* currentUser, User* userList) {
    limpiarPantalla();
    printf("--- BORRAR TWEET ---\n");

    StringNode* p = currentUser->tweets;
    if (p == NULL) {
        printf("No tienes tweets para borrar.\n");
        presionaXParaVolver();
        return;
    }

    // 1. Mostrar los tweets enumerados
    int i = 1;
    
    while (p != NULL) {
        printf("%d) [%s] %s\n", i, (p->fecha ? p->fecha : "N/A"), p->data);
        p = p->sig;
        i++;
    }
    int maxTweets = i - 1;

    printf("------------------------\n");
    printf(" X) Cancelar\n");
    printf("Elige el numero del tweet a borrar: ");
    
    char buffer[10];
    leerEntrada(buffer, 10);
    if (tolower(buffer[0]) == 'x') return;

    int opcion;
    if (sscanf(buffer, "%d", &opcion) != 1 || opcion < 1 || opcion > maxTweets) {
        printf("Opcion no valida.\n");
        Sleep(1000);
        return;
    }

    // 2. Lógica de borrado en Lista Enlazada
    StringNode* borrado = NULL;

    if (opcion == 1) {
        // CASO 1: Borrar el primer tweet (la cabeza de la lista)
        borrado = currentUser->tweets;          // Apuntamos al nodo a borrar
        currentUser->tweets = currentUser->tweets->sig; // Movemos la cabeza al siguiente
    } else {
        // CASO 2: Borrar un tweet intermedio o final
        StringNode* anterior = currentUser->tweets;
        int k;
        // Avanzamos hasta el nodo ANTERIOR al que queremos borrar
        for (k = 1; k < opcion - 1; k++) {
            anterior = anterior->sig;
        }
        borrado = anterior->sig;       // El nodo a borrar es el siguiente del anterior
        anterior->sig = borrado->sig;  // "Saltamos" el nodo a borrar en la cadena
    }

    // 3. Liberar memoria y guardar cambios
    if (borrado != NULL) {
        printf("\nBorrando tweet: '%s'...\n", borrado->data);
        free(borrado->data);
        if (borrado->fecha) free(borrado->fecha);
        free(borrado);
        
        guardarDatos(userList); // Actualizamos el archivo .txt
        printf("Tweet eliminado con exito.\n");
    }

    Sleep(1500);
}

User* cargarDatos() {
    FILE* f_users;
    User* head = NULL;
    char line[MAX_BUFFER];

    // Cargar Usuarios y Contraseñas (Se mantiene con coma)
    f_users = fopen("usuarios.txt", "r");
    if (f_users == NULL) {
        printf("No se encontro 'usuarios.txt'. Se creara uno nuevo al guardar.\n");
        Sleep(2000);
        return NULL; 
    }

    while (fgets(line, MAX_BUFFER, f_users) != NULL) {
        line[strcspn(line, "\n")] = 0; 
        char* user = strtok(line, ",");
        char* pass = strtok(NULL, ",");
        
        if (user && pass) {
            addUserToList(&head, createUser(user, pass));
        }
    }
    fclose(f_users);

    // Cargar Tweets (MODIFICADO PARA LEER PIPES '|')
    FILE* f_tweets = fopen("usuarios_tweet.txt", "r");
    if (f_tweets != NULL) {
        while (fgets(line, MAX_BUFFER, f_tweets) != NULL) {
            line[strcspn(line, "\n")] = 0;
            char* user = strtok(line, "|"); // <--- CAMBIO AQUÍ
            if (user == NULL) continue;
            
            User* pUser = findUser(head, user);
            if (pUser != NULL) {
                char* fechaToken;
                // Ahora leemos en pares: Primero Fecha, luego Tweet
                while ((fechaToken = strtok(NULL, "|")) != NULL) {
                    char* tweetToken = strtok(NULL, "|");
                    if (tweetToken != NULL) {
                        addStringNode(&(pUser->tweets), tweetToken, fechaToken);
                    }
                }
            }
        }
        fclose(f_tweets);
    }

    // Cargar Follows (MODIFICADO PARA LEER PIPES '|')
    FILE* f_follows = fopen("usuarios_follows.txt", "r");
    if (f_follows != NULL) {
        while (fgets(line, MAX_BUFFER, f_follows) != NULL) {
            line[strcspn(line, "\n")] = 0;
            char* user = strtok(line, "|"); // <--- CAMBIO AQUÍ
            if (user == NULL) continue;
            
            User* pUser = findUser(head, user);
            if (pUser != NULL) {
                char* follow;
                while ((follow = strtok(NULL, "|")) != NULL) { // <--- CAMBIO AQUÍ
                    addStringNode(&(pUser->following), follow, NULL);
                }
            }
        }
        fclose(f_follows);
    }

    return head;
}

// Otras Funciones---

void limpiarPantalla() {
    system("cls"); // Comando de Windows
}

void leerEntrada(char* buffer, int size) {
    fgets(buffer, size, stdin);
    buffer[strcspn(buffer, "\n")] = 0; // Quita el 'Enter'
}

void presionaXParaVolver() {
    printf("\nPresiona X para volver... ");
    char opcion;
    do {
        opcion = getOpcion();
    } while (opcion != 'x');
}

char getOpcion() {
    char buffer[10];
    leerEntrada(buffer, 10);
    return tolower(buffer[0]);
}



