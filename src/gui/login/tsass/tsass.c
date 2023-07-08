#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <openssl/sha.h>

#define MAX_USERS 10
#define MAX_USERNAME_LENGTH 20
#define MAX_PASSWORD_LENGTH 20
#define SALT "somesaltvalue"
#define LOCKOUT_DURATION 60 // Duración del bloqueo en segundos

typedef struct {
    char username[MAX_USERNAME_LENGTH];
    char password[MAX_PASSWORD_LENGTH];
    bool locked;
    int failed_attempts;
    time_t lockout_time;
} User;

User users[MAX_USERS] = {
    {"user1", "password1", false, 0, 0},
    {"user2", "password2", false, 0, 0},
    // Agrega más usuarios aquí si es necesario
};

bool is_user_locked(User* user) {
    if (user->lockout_time == 0)
        return false;

    time_t current_time = time(NULL);
    return (current_time - user->lockout_time) < LOCKOUT_DURATION;
}

void lock_user(User* user) {
    user->lockout_time = time(NULL);
    user->failed_attempts = 0;
    user->locked = true;
    printf("La cuenta de usuario '%s' ha sido bloqueada debido a múltiples intentos fallidos.\n", user->username);
}

void unlock_user(User* user) {
    user->lockout_time = 0;
    user->failed_attempts = 0;
    user->locked = false;
    printf("La cuenta de usuario '%s' ha sido desbloqueada.\n", user->username);
}

bool authenticate_user(const char* username, const char* password) {
    for (int i = 0; i < MAX_USERS; i++) {
        User* user = &users[i];
        if (strcmp(user->username, username) == 0) {
            if (is_user_locked(user)) {
                printf("La cuenta de usuario '%s' está bloqueada. Por favor, espere e intente nuevamente más tarde.\n", username);
                return false;
            }

            if (strcmp(user->password, password) == 0) {
                user->failed_attempts = 0; // Restablecer contador de intentos fallidos
                return true; // Usuario autenticado exitosamente
            }

            user->failed_attempts++;
            if (user->failed_attempts >= MAX_FAILED_ATTEMPTS) {
                lock_user(user);
                return false; // Autenticación fallida y cuenta bloqueada
            }

            printf("Contraseña incorrecta. Intento %d de %d.\n", user->failed_attempts, MAX_FAILED_ATTEMPTS);
            return false; // Autenticación fallida
        }
    }

    printf("El nombre de usuario '%s' no existe.\n", username);
    return false; // Autenticación fallida
}

void generate_hash(const char* password, unsigned char* hash) {
    char salted_password[MAX_PASSWORD_LENGTH + sizeof(SALT)];
    strcpy(salted_password, password);
    strcat(salted_password, SALT);

    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, salted_password, strlen(salted_password));
    SHA256_Final(hash, &sha256);
}

bool authenticate_user(const char* username, const char* password) {
    for (int i = 0; i < MAX_USERS; i++) {
        if (strcmp(users[i].username, username) == 0) {
            unsigned char hashed_password[SHA256_DIGEST_LENGTH];
            generate_hash(password, hashed_password);

            if (memcmp(users[i].hashed_password, hashed_password, SHA256_DIGEST_LENGTH) == 0) {
                return true; // Usuario autenticado exitosamente
            }
        }
    }
    return false; // Autenticación fallida
}

int main() {
    // Lógica principal del proceso tsass

    // Lógica de autenticación de usuarios
    char username[MAX_USERNAME_LENGTH];
    char password[MAX_PASSWORD_LENGTH];

    printf("Ingrese su nombre de usuario: ");
    scanf("%s", username);

    printf("Ingrese su contraseña: ");
    scanf("%s", password);

    if (authenticate_user(username, password)) {
        printf("¡Autenticación exitosa! Bienvenido, %s.\n", username);
        // Resto de la lógica del proceso después de la autenticación exitosa
    } else {
        printf("Autenticación fallida. Nombre de usuario o contraseña incorrectos.\n");
        // Resto de la lógica del proceso después de la autenticación fallida
    }

// Función para autenticar un usuario
bool authenticate_user(const char* username, const char* password) {
    for (int i = 0; i < MAX_USERS; i++) {
        if (strcmp(users[i].username, username) == 0) {
            unsigned char hashed_password[SHA256_DIGEST_LENGTH];
            generate_hash(password, hashed_password);
            
            if (memcmp(users[i].hashed_password, hashed_password, SHA256_DIGEST_LENGTH) == 0) {
                return true; // Usuario autenticado exitosamente
            }
        }
    }
    return false; // Autenticación fallida
}
