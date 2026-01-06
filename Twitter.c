#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> 
#include <windows.h> 
#define MAX_BUFFER 2048

// Estructuras
struct StringNode {
    char* data;
    struct StringNode* sig;
};
typedef struct StringNode StringNode;

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
StringNode* createStringNode(const char* data);
void addStringNode(StringNode** head, const char* data);
User* createUser(const char* user, const char* pass);
void addUserToList(User** head, User* nvoUser);
User* findUser(User* head, const char* username);
int isFollowing(User* currentUser, const char* username);

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

StringNode* createStringNode(const char* data) {
    StringNode* nvo = (StringNode*)malloc(sizeof(StringNode));
    nvo->data = (char*)malloc(strlen(data) + 1);
    strcpy(nvo->data, data);
    nvo->sig = NULL;
    return nvo;
}

// Inserta un nodo string final de la lista
void addStringNode(StringNode** head, const char* data) {
    StringNode* nvo = createStringNode(data);
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
        free(aux->data); // Libera la cadena
        free(aux);       // Libera el nodo
    }
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
    char tweet[280]; // Límite de Twitter
    
    limpiarPantalla();
    printf("Escribe tu tweet (max 280 caracteres):\n");
    leerEntrada(tweet, 280);

    if (strlen(tweet) == 0) {
        printf("No se puede publicar un tweet vacio.\n");
    } else {
        // Añade el tweet a la lista en memoria
        addStringNode(&(currentUser->tweets), tweet);
        
        // Guarda la estructura de datos completa en los archivos
        guardarDatos(userList);
        
        printf("\nTWEET PUBLICADO\n");
    }
    
    Sleep(1000);

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
            printf("\nTweet %d:\n", i);
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
        addStringNode(&(currentUser->following), usuarioASeleccionar->username);
        
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
    limpiarPantalla();
    printf("---------------------------\n");
    printf("         TWEETS\n");
    printf("---------------------------\n");
    
// Almacenamiento temporal en la pila.
// Asumimos máx. 100 personas seguidas 
//y máx. 200 tweets por persona.
    User* followedUsers[100];
    StringNode* tweetPointers[100][200];
    int tweetCounts[100] = {0}; // Inicializar contadores a 0
    int numFollowing = 0;
    int k,j;
    int maxTweets = 0; 
	// El N° de tweets del usuario con más tweets
    StringNode* pFollow = currentUser->following;

    // Construcción de los arreglos temporales
    while (pFollow != NULL && numFollowing < 100) {
        User* user = findUser(userList, pFollow->data);
        if (user) {
            followedUsers[numFollowing] = user;
            
            StringNode* pTweet = user->tweets;
            int count = 0;
            while (pTweet != NULL && count < 200) {
                tweetPointers[numFollowing][count] = pTweet;
                count++;
                pTweet = pTweet->sig;
            }
            
            tweetCounts[numFollowing] = count;
            if (count > maxTweets) {
                maxTweets = count;
            }
            
            numFollowing++;
        }
        pFollow = pFollow->sig;
    }
    //Lógica de Impresión (Round-Robin) ---    
    if (numFollowing == 0) {
        printf("Aun no sigues a nadie.\n");
    } else if (maxTweets == 0) {
        printf("Las personas que sigues no han publicado nada.\n");
    }

    int tweetsMostrados = 0;

//"k" es la ronda(0 = la más reciente, 1 = la 2da más reciente, etc.)
    for (k = 0; k < maxTweets; k++) {
        
// "j" es el usuario en la lista de seguidos (Ana, Pablo, José...)
        for (j = 0; j < numFollowing; j++) {
            
        // Calculamos el índice del tweet (N-ésimo desde el final)
        // ej. Si tiene 3 tweets (índices 0,1,2) y k=0:
        // tweetIndex = 3 - 1 - 0 = 2 (El último tweet)
            int tweetIndex = tweetCounts[j] - 1 - k;

            // Si este usuario todavía tiene un tweet en esta ronda
            if (tweetIndex >= 0) {
                User* user = followedUsers[j];
                StringNode* tweet = tweetPointers[j][tweetIndex];
                
                printf("TWEET DE %s:\n", user->username);
                printf("%s\n", tweet->data);
                printf("---------------------------\n");
                tweetsMostrados++;
            }
        }
    }
    
    // 3. Volver
    presionaXParaVolver();
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
            fprintf(f_tweets, "|%s", pTweet->data); // <--- CAMBIO AQUÍ
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
                char* tweet;
                while ((tweet = strtok(NULL, "|")) != NULL) { // <--- CAMBIO AQUÍ
                    addStringNode(&(pUser->tweets), tweet);
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
                    addStringNode(&(pUser->following), follow);
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

char getOpcion() {
    char buffer[10];
    leerEntrada(buffer, 10);
    return tolower(buffer[0]);
}

void presionaXParaVolver() {
    printf("\nPresiona X para volver... ");
    char opcion;
    do {
        opcion = getOpcion();
    } while (opcion != 'x');
}
