/*
 * plik zawiera deklaracje zmiennych globalnych
 *
 */

#ifndef GLOBALS_H
#define GLOBALS_H

#define ILOSC_LIST_WYSWIETLANIA 8

#include <windows.h>
#include <gl/gl.h>

// zmienne zawieraja listy wyswietlania poszczegolny elementow labiryntu
extern GLuint pole_mur;
extern GLuint pole_puste;
extern GLuint sciana_N; // NORTH
extern GLuint sciana_S;
extern GLuint sciana_W;
extern GLuint sciana_E;
extern GLuint sciana_N_exit;
extern GLuint sciana_W_exit;

#endif