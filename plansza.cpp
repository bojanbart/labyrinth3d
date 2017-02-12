#include "plansza.h"
#include "globals.h"
#include <ctime>
#include <cstdlib>
#include <cmath>

#define WARTOSC_KOREKTY 0.1f

const float piover180 = 0.0174532925f;

plansza::plansza(){
	// inicjalizacja generatora losowego
	srand(time(NULL));

	// ustawienie poczatkowej pozycji swiatla
	LightPosition[0]=0.0f;
	LightPosition[1]=1.5f;
	LightPosition[2]=0.0f;
	LightPosition[3]=1.0f;

	// restart planszy
	reset();
}

void plansza::reset(){
	// wylosowanie nowych wymiarow planszy
	wym_i=rand()%16+10; // z zakresu <10;25>
	wym_j=rand()%16+10;

	// ustawienie zerowego obrotu
	yrot=0.0f;

	// ustawienie pozycji obserwatora
	poz_i=wym_i-0.5f;
	poz_j=wym_j-0.5f;

	// ustawienie swiatla
	LightPosition[0]=poz_i*2;
	LightPosition[2]=poz_j*2;

	glLightfv(GL_LIGHT1, GL_POSITION,LightPosition);

	// ustawienie wyjscia labiryntu
	wyj_i=0.5f;
	wyj_j=0.5f;

	// pierwszy obieg petli
	bool pierwszy=true;

	rodzajPola r_pol;

	for(int i=0;i<wym_i;i++){
		for(int j=0;j<wym_j;j++){
			
			// okreslenie rodzaju pola
			r_pol=(rand()%3 ==0) ? zajete : wolne;

			if(j==wym_j-1 && i==wym_i-1) // wejscie do labiryntu
			{
				core[i][j].ustaw(2.0f, 0.0f, wolne);
			}else
			if(j==0){ // pierwsze pole z wiersza
				if(pierwszy){
					pierwszy=false;
					core[i][j].ustaw(1.0f,1.0f, wolne); // bo gorny lewy rog jest wyjsciem
														// i musi byc pusty
				}else{
					core[i][j].ustaw(-(wym_j-1)*2,2.0f, r_pol);
				}
			}else{ // pozostale pola
				core[i][j].ustaw(2.0f, 0.0f, r_pol);
			}
		}
	}

	if(!doPrzejscia()) reset();
}

void plansza::rysuj(){
	// obrot wokol osi y
	glRotatef(yrot, 0.0f, 1.0f, 0.0f);

	// przesuniecie o pozycje obserwatora
	glTranslatef(-poz_j*2,0.0f,-poz_i*2);

	for(int i=0;i<wym_i;i++)
		for(int j=0;j<wym_j;j++){
			core[i][j].rysuj();
			if(i==0 && j==0){ // rysowanie tekstury sciany wyjscia
				glCallList(sciana_N_exit);
				glCallList(sciana_W_exit);
			}else{
				if(i==0) glCallList(sciana_N);
				if(i==wym_i-1) glCallList(sciana_S);
				if(j==0) glCallList(sciana_W);
				if(j==wym_j-1) glCallList(sciana_E);
			}
		}
}

void plansza::obroc(GLfloat o_ile){
	yrot+=o_ile;
}

void plansza::ruch_UP(){
	GLfloat poz_i_new, poz_j_new;
	poz_i_new = poz_i-(float)cos(yrot*piover180) * 0.05f;
	poz_j_new = poz_j+(float)sin(yrot*piover180) * 0.05f;
	if(poz_i_new>=0.1f && poz_i_new<=wym_i-0.1f && poz_j_new>=0.1f && poz_j_new<=wym_j-0.1f &&
		moznaWejsc(poz_i_new, poz_j_new)){
		poz_i=poz_i_new;
		poz_j=poz_j_new;
	}
	czyKoniec();
}

void plansza::ruch_DOWN(){
	GLfloat poz_i_new, poz_j_new;
	poz_i_new = poz_i + (float)cos(yrot*piover180) * 0.05f;
	poz_j_new = poz_j - (float)sin(yrot*piover180) * 0.05f;
	if(poz_i_new>=0.1f && poz_i_new<=wym_i-0.1f && poz_j_new>=0.1f && poz_j_new<=wym_j-0.1f &&
		moznaWejsc(poz_i_new, poz_j_new)){
		poz_i=poz_i_new;
		poz_j=poz_j_new;
	}
	czyKoniec();
}

// zaklada, ze wejscie i wyjscie z labiryntu jest polem pustym
bool plansza::doPrzejscia(){
	reset_odwiedzony();
    punkt pun;
    pun.i=wym_i-1;
    pun.j=wym_j-1;
    odwiedzony[pun.i][pun.j]=true;
    return analizujPole(pun);
}

void plansza::reset_odwiedzony(){
     for(int i=0;i<wym_i;i++)
             for(int j=0;j<wym_j;j++)
                     odwiedzony[i][j]=false;
}

bool plansza::analizujPole(punkt p){
	 if(p.i==0 && p.j==0) return true;
     odwiedzony[p.i][p.j]=true;
     
     punkt sasiedzi[ILOSC_SASIADOW];
     
     sasiedzi[SASIAD_N].i=p.i-1;
     sasiedzi[SASIAD_N].j=p.j;
     sasiedzi[SASIAD_E].i=p.i+1;
     sasiedzi[SASIAD_E].j=p.j;
     sasiedzi[SASIAD_S].i=p.i+1;
     sasiedzi[SASIAD_S].j=p.j;
     sasiedzi[SASIAD_W].i=p.i;
     sasiedzi[SASIAD_W].j=p.j-1;
     
     for(int x=0;x<ILOSC_SASIADOW;x++){
             if(sasiedzi[x].i >=0 && sasiedzi[x].i < wym_i && 
                sasiedzi[x].j >=0 && sasiedzi[x].j < wym_j ){
                              if(!odwiedzony[sasiedzi[x].i][sasiedzi[x].j] && 
								  core[sasiedzi[x].i][sasiedzi[x].j].jakiRodzaj()==wolne && 
                              analizujPole(sasiedzi[x])) return true;
                              }
             }
             
     return false;
}

void plansza::czyKoniec(){
	if(poz_i<0.99f && poz_j<0.99f) reset();
}

bool plansza::moznaWejsc(GLfloat wsp_i, GLfloat wsp_j){

	// zmienne na korekte, zapobiegaja wychylania kamery za sciany
	GLfloat korekta_i=0.0f;
	GLfloat korekta_j=0.0f;

	// ustawienie korekty
	if(wsp_i>poz_i) korekta_i=WARTOSC_KOREKTY;
	if(wsp_i<poz_i) korekta_i=-WARTOSC_KOREKTY;
	if(wsp_j>poz_j) korekta_j=WARTOSC_KOREKTY;
	if(wsp_j<poz_j) korekta_j=-WARTOSC_KOREKTY;   

	int wsp_i_=static_cast<int>(wsp_i+korekta_i);
	int wsp_j_=static_cast<int>(wsp_j+korekta_j);
	if(wsp_i_==wym_i) wsp_i_=wym_i-1;
	if(wsp_j_==wym_j) wsp_j_=wym_j-1;
	if(core[wsp_i_][wsp_j_].jakiRodzaj()==zajete) return false;
	return true;
}