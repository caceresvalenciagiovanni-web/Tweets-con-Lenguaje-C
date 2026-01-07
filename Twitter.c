#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> 
#include <windows.h> 
#include <time.h>
#define MAX_BUFFER 2048

// --- ESTRUCTURAS ---
struct StringNode {
    char* data;           
    char* fecha;          
    struct StringNode* sig;
};
typedef struct StringNode StringNode;

struct TweetDisplay {
    char autor[50];
    StringNode* tweet;
};
typedef struct TweetDisplay TweetDisplay;

struct User {
    char username[50];
    char password[50];
    StringNode* tweets;    
    StringNode* following; 
    struct User* sig;      
};
typedef struct User User;

// --- PROTOTIPOS ---
// Utilidad
StringNode* createStringNode(const char* data, const char* fecha);
void addStringNode(StringNode** head, const char* data, const char* fecha);
void borrarStringNode(StringNode** head, const char* data); // NUEVA
User* createUser(const char* user, const char* pass);
void addUserToList(User** head, User* nvoUser);
User* findUser(User* head, const char* username);
int isFollowing(User* currentUser, const char* username);
int compararFechas(const char* f1, const char* f2);

// Archivos y Memoria
void guardarDatos(User* head);
User* cargarDatos();
void liberarStringList(StringNode* head);
void liberarUserList(User* head);

// Menús Principales (Interfaces)
void menuBienvenida(User** userList);
User* menuLogin(User* userList);
User* menuCrearUsuario(User** userList);

// LA NUEVA ESTRUCTURA DE NAVEGACIÓN
void menuTwitter(User* currentUser, User* userList);   // Interfaz 1
void menuPerfil(User* currentUser, User* userList);    // Interfaz 2
void menuAmistades(User* currentUser, User* userList); // Interfaz 3

// Acciones Específicas
void accionPublicarTweet(User* currentUser, User* userList);
void accionRetwittear(User* currentUser, User* userList);
void accionBorrarTweet(User* currentUser, User* userList);
void accionSeguir(User* currentUser, User* userList);
void accionDejarDeSeguir(User* currentUser, User* userList); // NUEVA
void accionVerSeguidores(User* currentUser, User* userList);

// Auxiliares
void limpiarPantalla();
void leerEntrada(char* buffer, int size);
char getOpcion();
void presionaXParaVolver();

// --- MAIN ---
int main() {
    SetConsoleCP(1252);
    SetConsoleOutputCP(1252);
    
    User* userList = cargarDatos(); 
    menuBienvenida(&userList); 
    liberarUserList(userList);
    
    printf("Gracias por usar Twitter. ¡Adiós!\n");
    return 0;
}

// --- IMPLEMENTACIÓN DE LISTAS ---
StringNode* createStringNode(const char* data, const char* fecha) {
    StringNode* nvo = (StringNode*)malloc(sizeof(StringNode));
    nvo->data = (char*)malloc(strlen(data) + 1);
    strcpy(nvo->data, data);
    
    if (fecha != NULL) {
        nvo->fecha = (char*)malloc(strlen(fecha) + 1);
        strcpy(nvo->fecha, fecha);
    } else {
        nvo->fecha = NULL;
    }
    nvo->sig = NULL;
    return nvo;
}

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

// Función para eliminar un nodo específico (Usada para dejar de seguir)
void borrarStringNode(StringNode** head, const char* data) {
    if (*head == NULL) return;

    StringNode* curr = *head;
    StringNode* prev = NULL;

    // Si es el primero
    if (strcmp(curr->data, data) == 0) {
        *head = curr->sig;
        free(curr->data);
        if (curr->fecha) free(curr->fecha);
        free(curr);
        return;
    }

    // Si está en medio o final
    while (curr != NULL && strcmp(curr->data, data) != 0) {
        prev = curr;
        curr = curr->sig;
    }

    if (curr == NULL) return; // No encontrado

    prev->sig = curr->sig;
    free(curr->data);
    if (curr->fecha) free(curr->fecha);
    free(curr);
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
            return p;
        }
        p = p->sig;
    }
    return NULL; 
}

int isFollowing(User* currentUser, const char* username) {
    StringNode* p = currentUser->following;
    while (p != NULL) {
        if (strcmp(p->data, username) == 0) {
            return 1; 
        }
        p = p->sig;
    }
    return 0; 
}

void liberarStringList(StringNode* head) {
    StringNode* aux;
    while (head != NULL) {
        aux = head;
        head = head->sig;
        free(aux->data);
        if (aux->fecha != NULL) free(aux->fecha); 
        free(aux);
    }
}

void liberarUserList(User* head) {
    User* aux;
    while (head != NULL) {
        aux = head;
        head = head->sig;
        liberarStringList(aux->tweets);
        liberarStringList(aux->following);
        free(aux);
    }
}

int compararFechas(const char* f1, const char* f2) {
    if (f1 == NULL) return -1;
    if (f2 == NULL) return 1;
    int d1, m1, a1, h1, min1;
    int d2, m2, a2, h2, min2;
    sscanf(f1, "%d/%d/%d %d:%d", &d1, &m1, &a1, &h1, &min1);
    sscanf(f2, "%d/%d/%d %d:%d", &d2, &m2, &a2, &h2, &min2);
    if (a1 > a2) return 1; if (a1 < a2) return -1;
    if (m1 > m2) return 1; if (m1 < m2) return -1;
    if (d1 > d2) return 1; if (d1 < d2) return -1;
    if (h1 > h2) return 1; if (h1 < h2) return -1;
    if (min1 > min2) return 1; if (min1 < min2) return -1;
    return 0; 
}

// --- MENÚS DE ENTRADA ---
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
            case 'x': break;
            default: printf("Opcion no valida...\n"); Sleep(1000);
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

    if (findUser(*userList, nvoUser) != NULL) {
        printf("Error: El usuario '%s' ya existe.\n", nvoUser);
        Sleep(2000);
        return NULL;
    }
    printf("Crea una contraseña: ");
    leerEntrada(nvoPass, 50);

    User* nvo = createUser(nvoUser, nvoPass);
    addUserToList(userList, nvo);
    guardarDatos(*userList);
    
    printf("-------------------------\n");
    printf("¡Usuario '%s' creado con exito!\n", nvoUser);
    printf("-------------------------\n");
    Sleep(1500);
    return nvo;
}

User* menuLogin(User* userList) {
    char username[50], password[50];
    while (1) { 
        limpiarPantalla();
        printf("-------------------------\n");
        printf("--- Iniciar Sesion ---\n");
        printf("-------------------------\n");
        printf("Usuario: ");
        leerEntrada(username, 50);

        User* usuarioEncontrado = findUser(userList, username);

        if (usuarioEncontrado == NULL) {
            printf("Usuario no encontrado.\n");
            printf("-------------------------\n");
            printf("¿Deseas crear uno nuevo?\n");
            printf(" 1) SI\n 2) NO (Reintentar)\n X) Salir\n");
            printf("Opcion: ");
            char opcion = getOpcion();
            if (opcion == '1') return menuCrearUsuario(&userList);
            else if (opcion == 'x') return NULL;
        } else {
            while (1) { 
                printf("Contrasena: ");
                leerEntrada(password, 50);
                if (strcmp(usuarioEncontrado->password, password) == 0) {
                    printf("¡Bienvenido, %s!\n", usuarioEncontrado->username);
                    Sleep(1500);
                    return usuarioEncontrado;
                } else {
                    printf("-------------------------\n");
                    printf("Contrasena Incorrecta.\n");
                    printf("¿Que deseas hacer?\n");
                    printf(" 1) VOLVER A ESCRIBIR\n X) Salir\n");
                    printf("Opcion: ");
                    char opcion = getOpcion();
                    if (opcion == 'x') return NULL;
                }
            }
        }
    }
}

// ==========================================================
//    INTERFAZ 1: FEED PRINCIPAL (MENU TWITTER)
// ==========================================================
void menuTwitter(User* currentUser, User* userList) {
    char opcion;
    int i,j;
    do {
        limpiarPantalla();
        printf("---------------------------\n");
        printf("         TWITTER\n");
        printf("---------------------------\n");
        printf("1) Twittear \n");
        printf("2) ReTwittear \n");
        printf("3) Mi Perfil  \n");
        printf("X) Cerrar Sesion\n");
        printf("---------------------------\n");
        printf("FEED PRINCIPAL\n");
        printf("---------------------------\n");

        // --- MOSTRAR FEED (Sin números, solo lectura) ---
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

        // Ordenamiento
        for (i = 0; i < totalTweets - 1; i++) {
            for (j = 0; j < totalTweets - i - 1; j++) {
                if (compararFechas(feed[j].tweet->fecha, feed[j+1].tweet->fecha) < 0) {
                    TweetDisplay temp = feed[j];
                    feed[j] = feed[j+1];
                    feed[j+1] = temp;
                }
            }
        }

        // Impresión
        if (totalTweets == 0) {
            printf("  (No hay tweets para mostrar)\n");
            printf("---------------------------\n");
        } else {
            for (i = 0; i < totalTweets; i++) {
                StringNode* t = feed[i].tweet;
                printf("@%s [%s]:\n", feed[i].autor, (t->fecha ? t->fecha : "S/F"));
                printf("   %s\n", t->data);
                printf("---------------------------\n");
            }
        }

        printf("Selecciona una opcion: ");
        opcion = getOpcion();

        switch (opcion) {
            case '1':
                accionPublicarTweet(currentUser, userList);
                break;
            case '2':
                accionRetwittear(currentUser, userList);
                break;
            case '3':
                menuPerfil(currentUser, userList); // Ir a Interfaz 2
                break;
            case 'x':
                return; // Cerrar sesión
            default:
                break;
        }
    } while (opcion != 'x');
}

// ==========================================================
//    INTERFAZ 2: MI PERFIL
// ==========================================================
void menuPerfil(User* currentUser, User* userList) {
    char opcion;
    int i,j;
    do {
        limpiarPantalla();
        printf("---------------------------------\n");
        printf("         MI PERFIL\n");
        printf("---------------------------------\n");
        printf("1) Borrar un Tweet\n");
        printf("2) Amistades  \n");
        printf("x) Regresar\n");
        printf("---------------------------------\n");
        printf("         MIS TWEETS\n");
        printf("---------------------------------\n");

        // --- MOSTRAR MIS TWEETS (Sin números) ---
        StringNode* misTweets[500];
        int count = 0;
        StringNode* p = currentUser->tweets;

        while (p != NULL && count < 500) {
            misTweets[count] = p;
            count++;
            p = p->sig;
        }
        
        // Ordenamiento
        for (i = 0; i < count - 1; i++) {
            for (j = 0; j < count - i - 1; j++) {
                if (compararFechas(misTweets[j]->fecha, misTweets[j+1]->fecha) < 0) {
                    StringNode* temp = misTweets[j];
                    misTweets[j] = misTweets[j+1];
                    misTweets[j+1] = temp;
                }
            }
        }

        // Impresión
        if (count == 0) {
            printf("  (Aun no has publicado nada)\n");
            printf("---------------------------------\n");
        } else {
            for (i = 0; i < count; i++) {
                StringNode* t = misTweets[i];
                printf("[%s]\n", (t->fecha ? t->fecha : "S/F"));
                printf(" %s\n", t->data);
                printf("---------------------------------\n");
            }
        }

        printf("Selecciona una opcion: ");
        opcion = getOpcion();

        switch (opcion) {
            case '1':
                accionBorrarTweet(currentUser, userList);
                break;
            case '2':
                menuAmistades(currentUser, userList); // Ir a Interfaz 3
                break;
            case 'x':
                return; // Regresar a Menu Twitter
            default:
                break;
        }

    } while (opcion != 'x');
}

// ==========================================================
//    INTERFAZ 3: AMISTADES
// ==========================================================
void menuAmistades(User* currentUser, User* userList) {
    char opcion;
    do {
        limpiarPantalla();
        printf("---------------------------------\n");
        printf("          AMISTADES\n");
        printf("---------------------------------\n");
        printf("1) Seguir a alguien\n");
        printf("2) Dejar de seguir a alguien\n");
        printf("3) Ver a mis seguidores  \n");
        printf("x) Regresar\n");
        printf("---------------------------------\n");
        printf("    ACTUALMENTE SIGUES A\n");
        printf("---------------------------------\n");

        StringNode* p = currentUser->following;
        if (p == NULL) {
            printf("  (No sigues a nadie aun)\n");
        } else {
            while (p != NULL) {
                printf("- %s\n", p->data);
                p = p->sig;
            }
        }
        printf("--------------------------------\n");
        printf("Selecciona una opcion: ");
        opcion = getOpcion();

        switch (opcion) {
            case '1':
                accionSeguir(currentUser, userList);
                break;
            case '2':
                accionDejarDeSeguir(currentUser, userList);
                break;
            case '3':
                accionVerSeguidores(currentUser, userList);
                break;
            case 'x':
                return; // Regresar a Mi Perfil
            default:
                break;
        }

    } while (opcion != 'x');
}

// ==========================================================
//    ACCIONES ESPECÍFICAS (Lógica de las opciones)
// ==========================================================

void accionPublicarTweet(User* currentUser, User* userList) {
    char tweet[280]; 
    char fechaActual[50]; 
    
    limpiarPantalla();
    printf("---------------------------\n");
    printf("        TWITTER\n");
    printf("---------------------------\n");
    printf("1) Twittear \n");
    printf("2) ReTwittear \n");
    printf("3) Mi Perfil  \n");
    printf("X) Cerrar Sesion\n");
    printf("---------------------------\n");
    printf("PUBLICAR UN TWEET\n");
    printf("---------------------------\n");
    printf("Escribe tu tweet (max 280 caracteres):\n");
    leerEntrada(tweet, 280);

    if (strlen(tweet) > 0) {
        time_t t = time(NULL);
        struct tm *tm = localtime(&t);
        strftime(fechaActual, sizeof(fechaActual), "%d/%m/%Y %H:%M", tm);

        addStringNode(&(currentUser->tweets), tweet, fechaActual);
        guardarDatos(userList);
        
        printf("\nTWEET PUBLICADO a las %s\n", fechaActual);
        Sleep(2000);
    }
}

void accionRetwittear(User* currentUser, User* userList) {
    limpiarPantalla();
    int i,j;
    printf("---------------------------------\n");
    printf("         TWITTER\n");
    printf("---------------------------\n");
    printf("1) Twittear \n");
    printf("2) ReTwittear \n");
    printf("3) Mi Perfil  \n");
    printf("x) Cerrar Sesion\n");
    printf("---------------------------\n");
    printf("RETWITTEAR\n");
    printf("---------------------------\n");

    // Recolección
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

    // Ordenamiento
    for (i = 0; i < totalTweets - 1; i++) {
        for (j = 0; j < totalTweets - i - 1; j++) {
            if (compararFechas(feed[j].tweet->fecha, feed[j+1].tweet->fecha) < 0) {
                TweetDisplay temp = feed[j];
                feed[j] = feed[j+1];
                feed[j+1] = temp;
            }
        }
    }

    // Impresión Enumerada
    for (i = 0; i < totalTweets; i++) {
        StringNode* t = feed[i].tweet;
        printf("%d) @%s [%s]:\n", i + 1, feed[i].autor, (t->fecha ? t->fecha : "S/F"));
        printf("   %s\n", t->data);
        printf("---------------------------\n");
    }

    printf("Elige una opcion o pulsa x para volver: ");
    char buffer[10];
    leerEntrada(buffer, 10);
    
    if (tolower(buffer[0]) == 'x') return;

    int numTweet;
    if (sscanf(buffer, "%d", &numTweet) == 1 && numTweet >= 1 && numTweet <= totalTweets) {
        TweetDisplay seleccionado = feed[numTweet - 1];
        char nuevoTexto[350]; 
        char fechaActual[50];

        snprintf(nuevoTexto, sizeof(nuevoTexto), "RT @%s: %s", 
                 seleccionado.autor, seleccionado.tweet->data);

        time_t t = time(NULL);
        struct tm *tm = localtime(&t);
        strftime(fechaActual, sizeof(fechaActual), "%d/%m/%Y %H:%M", tm);

        addStringNode(&(currentUser->tweets), nuevoTexto, fechaActual);
        guardarDatos(userList);

        printf("\nTWEET RETWITTEADO a las %s\n", fechaActual);
        Sleep(2000);
    }
}

void accionBorrarTweet(User* currentUser, User* userList) {
    limpiarPantalla();
    printf("---------------------------------\n");
    printf("         MI PERFIL\n");
    printf("---------------------------------\n");
    printf("1) Borrar un Tweet\n");
    printf("2) Amistades  \n");
    printf("x) Regresar\n");
    printf("---------------------------------\n");
    printf("        BORRAR TWEETS\n");
    printf("---------------------------------\n");

    // Para borrar, mostramos la lista en orden original (de memoria) 
    // para facilitar el borrado por índice de la lista enlazada.
    StringNode* p = currentUser->tweets;
    int i = 1;
    int k;
    while (p != NULL) {
        printf("%d) [%s] %s\n", i, (p->fecha ? p->fecha : "N/A"), p->data);
        p = p->sig;
        i++;
    }
    int maxTweets = i - 1;

    printf("--------------------------------\n");
    printf("Elige una opcion o pulsa x para volver: ");
    char buffer[10];
    leerEntrada(buffer, 10);
    if (tolower(buffer[0]) == 'x') return;

    int opcion;
    if (sscanf(buffer, "%d", &opcion) != 1 || opcion < 1 || opcion > maxTweets) {
        return;
    }

    // Borrado
    StringNode* borrado = NULL;
    if (opcion == 1) {
        borrado = currentUser->tweets; 
        currentUser->tweets = currentUser->tweets->sig; 
    } else {
        StringNode* anterior = currentUser->tweets;
        for (k = 1; k < opcion - 1; k++) {
            anterior = anterior->sig;
        }
        borrado = anterior->sig;       
        anterior->sig = borrado->sig; 
    }

    if (borrado != NULL) {
        printf("\nBorrando tweet: '%s'...\n", borrado->data);
        free(borrado->data);
        if (borrado->fecha) free(borrado->fecha);
        free(borrado);
        
        guardarDatos(userList); 
        printf("Tweet eliminado con exito.\n");
        Sleep(2000);
    }
}

void accionSeguir(User* currentUser, User* userList) {
    limpiarPantalla();
    printf("---------------------------------\n");
    printf("          AMISTADES\n");
    printf("---------------------------------\n");
    printf("1) Seguir a alguien\n");
    printf("2) Dejar de seguir a alguien  \n");
    printf("x) Regresar\n");
    printf("---------------------------------\n");
    printf("   ¿A QUIEN VAS A SEGUIR?\n");
    printf("---------------------------------\n");

    User* p = userList;
    User* usuariosDisponibles[100]; 
    int count = 0;

    while (p != NULL && count < 100) {
        if (strcmp(p->username, currentUser->username) != 0) {
            printf(" %d) %s\n", (count + 1), p->username);
            usuariosDisponibles[count] = p;
            count++;
        }
        p = p->sig;
    }
    printf(" x) Volver\n");
    printf("--------------------------------\n");
    printf("Elige una opcion: ");
    
    char buffer[10];
    leerEntrada(buffer, 10);
    if (tolower(buffer[0]) == 'x') return;

    int opcion;
    if (sscanf(buffer, "%d", &opcion) == 1 && opcion >= 1 && opcion <= count) {
        User* aSeguir = usuariosDisponibles[opcion - 1];
        if (isFollowing(currentUser, aSeguir->username)) {
            printf("Ya estas siguiendo a %s.\n", aSeguir->username);
        } else {
            addStringNode(&(currentUser->following), aSeguir->username, NULL);
            guardarDatos(userList);
            printf("Ahora estas siguiendo a %s.\n", aSeguir->username);
        }
        Sleep(2000);
    }
}

void accionDejarDeSeguir(User* currentUser, User* userList) {
    limpiarPantalla();
    printf("---------------------------------\n");
    printf("          AMISTADES\n");
    printf("---------------------------------\n");
    printf("1) Seguir a alguien\n");
    printf("2) Dejar de seguir a alguien\n");
    printf("3) Ver a mis seguidores  \n");
    printf("x) Regresar\n");
    printf("---------------------------------\n");
    printf(" ¿A QUIEN DEJARAS DE SEGUIR?\n");
    printf("---------------------------------\n");

    // Listamos a quien seguimos para poder seleccionar
    StringNode* p = currentUser->following;
    StringNode* seguidos[100];
    int count = 0;

    while (p != NULL && count < 100) {
        printf("%d) %s\n", count + 1, p->data);
        seguidos[count] = p;
        count++;
        p = p->sig;
    }

    if (count == 0) {
        printf("No sigues a nadie para borrar.\n");
        Sleep(2000);
        return;
    }

    printf("--------------------------------\n");
    printf("Elige una opcion o x para volver: ");
    
    char buffer[10];
    leerEntrada(buffer, 10);
    if (tolower(buffer[0]) == 'x') return;

    int opcion;
    if (sscanf(buffer, "%d", &opcion) == 1 && opcion >= 1 && opcion <= count) {
        char* nombreABorrar = seguidos[opcion - 1]->data;
        
        // Función auxiliar para eliminar nodo
        borrarStringNode(&(currentUser->following), nombreABorrar);
        
        guardarDatos(userList);
        printf("Has dejado de seguir a %s\n", nombreABorrar);
        Sleep(2000);
    }
}

void accionVerSeguidores(User* currentUser, User* userList) {
    limpiarPantalla();
    printf("---------------------------------\n");
    printf("          AMISTADES\n");
    printf("---------------------------------\n");
    printf("1) Seguir a alguien\n");
    printf("2) Dejar de seguir a alguien\n");
    printf("3) Ver a mis seguidores  \n");
    printf("x) Regresar\n");
    printf("---------------------------------\n");
    printf("    LISTA DE SEGUIDORES\n");
    printf("---------------------------------\n");

    User* p = userList;
    int count = 0;
    while (p != NULL) {
        if (strcmp(p->username, currentUser->username) != 0) {
            if (isFollowing(p, currentUser->username)) {
                printf("- %s\n", p->username);
                count++;
            }
        }
        p = p->sig;
    }
    if (count == 0) printf("(Aun no tienes seguidores)\n");
    
    printf("--------------------------------\n");
    printf("Escribe x para volver: ");
    char buffer[10];
    do {
        leerEntrada(buffer, 10);
    } while (tolower(buffer[0]) != 'x');
}

// Carga y Guardado ---
void guardarDatos(User* head) {
    FILE *f_users, *f_tweets, *f_follows;
    f_users = fopen("usuarios.txt", "w");
    f_tweets = fopen("usuarios_tweet.txt", "w");
    f_follows = fopen("usuarios_follows.txt", "w");

    if (!f_users || !f_tweets || !f_follows) exit(1);

    User* pUser = head;
    while (pUser != NULL) {
        fprintf(f_users, "%s,%s\n", pUser->username, pUser->password);

        fprintf(f_tweets, "%s", pUser->username);
        StringNode* pTweet = pUser->tweets;
        while (pTweet != NULL) {
            char* fechaStr = (pTweet->fecha != NULL) ? pTweet->fecha : "N/A";
            fprintf(f_tweets, "|%s|%s", fechaStr, pTweet->data);
            pTweet = pTweet->sig;
        }
        fprintf(f_tweets, "\n");

        fprintf(f_follows, "%s", pUser->username);
        StringNode* pFollow = pUser->following;
        while (pFollow != NULL) {
            fprintf(f_follows, "|%s", pFollow->data);
            pFollow = pFollow->sig;
        }
        fprintf(f_follows, "\n");
        pUser = pUser->sig;
    }
    fclose(f_users); fclose(f_tweets); fclose(f_follows);
}

User* cargarDatos() {
    FILE* f_users = fopen("usuarios.txt", "r");
    if (f_users == NULL) return NULL;
    User* head = NULL;
    char line[MAX_BUFFER];

    while (fgets(line, MAX_BUFFER, f_users)) {
        line[strcspn(line, "\n")] = 0; 
        char* user = strtok(line, ",");
        char* pass = strtok(NULL, ",");
        if (user && pass) addUserToList(&head, createUser(user, pass));
    }
    fclose(f_users);

    FILE* f_tweets = fopen("usuarios_tweet.txt", "r");
    if (f_tweets) {
        while (fgets(line, MAX_BUFFER, f_tweets)) {
            line[strcspn(line, "\n")] = 0;
            char* user = strtok(line, "|");
            if (!user) continue;
            User* pUser = findUser(head, user);
            if (pUser) {
                char* fechaToken;
                while ((fechaToken = strtok(NULL, "|"))) {
                    char* tweetToken = strtok(NULL, "|");
                    if (tweetToken) addStringNode(&(pUser->tweets), tweetToken, fechaToken);
                }
            }
        }
        fclose(f_tweets);
    }

    FILE* f_follows = fopen("usuarios_follows.txt", "r");
    if (f_follows) {
        while (fgets(line, MAX_BUFFER, f_follows)) {
            line[strcspn(line, "\n")] = 0;
            char* user = strtok(line, "|");
            if (!user) continue;
            User* pUser = findUser(head, user);
            if (pUser) {
                char* follow;
                while ((follow = strtok(NULL, "|"))) {
                    addStringNode(&(pUser->following), follow, NULL);
                }
            }
        }
        fclose(f_follows);
    }
    return head;
}

void limpiarPantalla() { system("cls"); }
void leerEntrada(char* buffer, int size) {
    fgets(buffer, size, stdin);
    buffer[strcspn(buffer, "\n")] = 0; 
}
char getOpcion() {
    char buffer[10];
    leerEntrada(buffer, 10);
    return tolower(buffer[0]);
}


