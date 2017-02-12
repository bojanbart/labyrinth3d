#include "pole.h"
#include "globals.h"

void pole::rysuj(){
	glTranslatef(przes_x, 0.0f, przes_z);

	switch(rodzaj){
		case wolne:
			glCallList(pole_puste);
			break;
		case zajete:
			glCallList(pole_mur);
			break;
	}
}

void pole::ustaw(GLfloat przes_x_, GLfloat przes_z_, rodzajPola rodzaj_){
	przes_x=przes_x_;
	przes_z=przes_z_;
	rodzaj=rodzaj_;
}