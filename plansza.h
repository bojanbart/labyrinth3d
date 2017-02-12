#ifndef PLANSZA_H
#define PLANSZA_H

#include <windows.h>
#include <gl/gl.h>
#include "pole.h"

#define SASIAD_N 0
#define SASIAD_E 1
#define SASIAD_S 2
#define SASIAD_W 3
#define ILOSC_SASIADOW 4

struct punkt{
       int i;
       int j;       
};

// klasa planszy do gry
class plansza{
public:
	// konstruktor
	plansza();

	// resetuje plansze
	void reset();

	void rysuj();

	// obraca obserwatora ( a raczej swiat )
	void obroc(GLfloat o_ile);

	// funkcje symulujace ruch kamery
	void ruch_UP();
	void ruch_DOWN();

private:
	
	// maksymalny rozmiar tablicy pol
	static const int max_i=25;
	static const int max_j=25;

	// okreslaja aktualny rozmiar plaszy
	int wym_i;
	int wym_j;

	// pozycja obserwatora
	GLfloat poz_i;
	GLfloat poz_j;

	// wspolrzedne wyjscia z labiryntu
	GLfloat wyj_i;
	GLfloat wyj_j;

	// obrot 
	GLfloat yrot;

	// plansza
	pole core[max_i][max_j];

	// pozycja swiatla
	GLfloat LightPosition[4];

	// funkcja okresla czy labirynt mozna przejsc
	bool doPrzejscia();

	// tablica okresla, ktore pola zostaly juz odwiedzone
	bool odwiedzony[max_i][max_j];

	// resetuje tablice odwiedzony
	void reset_odwiedzony();

	// funkcja analizujaca dane pole wywolywana przez doPezejscia
	bool analizujPole(punkt p);

	// funkcja sprawdza czy kamera znalazla sie w punkcie wyjscia
	void czyKoniec();

	// sprawdza czy czasem nie chce sie wejsc na pole mur
	// true - mozna wejsc, false - nie
	bool moznaWejsc(GLfloat wsp_i, GLfloat wsp_j);
};

#endif