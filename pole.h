#ifndef POLE_H
#define POLE_H

#include <windows.h>
#include <gl/gl.h>

// wyliczenie okreslajace rodzaj pola
enum rodzajPola { wolne /* sciezka po ktorej mozna chodzic */,
				zajete /* mur */ };

// klasa pola do gry
class pole{
public:
	// rysuje podane pole
	void rysuj();

	// ustawia parametry pola
	void ustaw(GLfloat przes_x_, GLfloat przes_z_, rodzajPola rodzaj_);

	// zwraca rodzaj pola
	rodzajPola jakiRodzaj(){
		return rodzaj;
	}
private:
	// zmienne okreslaja przesuniecie tego pola na planszy
	GLfloat przes_x;
	GLfloat przes_z;

	rodzajPola rodzaj;
};

#endif