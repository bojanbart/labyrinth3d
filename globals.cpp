#include <windows.h>
#include <gl/gl.h>

// zmienne zawieraja listy wyswietlania poszczegolny elementow labiryntu
GLuint pole_mur;
GLuint pole_puste;
GLuint sciana_N; // NORTH
GLuint sciana_S;
GLuint sciana_W;
GLuint sciana_E;
GLuint sciana_N_exit;
GLuint sciana_W_exit;