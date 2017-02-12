/*******************************************************************************
 * labirynt 3D
 *
 * Sterowanie:
 * kursory - ruch
 * N - nowy labirynt
 * L - wl/wyl swiatlo
 * F - wl/wyl mgle
 * ESC - wyjscie z programu
 *
 * Zalozenia:
 * wejscie do labiryntu: prawy dolny rog
 * wyjscie z labiryntu: lewy gorny rog
 *
 *******************************************************************************
 */

/*******************************************************************************
 * include
 *******************************************************************************
 */
#include <windows.h>
#include <cstdio>
#include <gl/gl.h>
#include <gl/glu.h>
#include <gl/glaux.h>
#include "globals.h"
#include "pole.h"
#include "plansza.h"


/*******************************************************************************
 * define
 *******************************************************************************
 */
#define ILOSC_TEKSTUR 4
#define TEKSTURA_PODLOGA 0
#define TEKSTURA_SCIANA 1
#define TEKSTURA_SUFIT 2
#define TEKSTURA_SCIANA_EXIT 3


/*******************************************************************************
 * globals
 *******************************************************************************
 */
HGLRC hRC=NULL;         // kontekst renderownia
HDC hDC=NULL;           // kontekst graficzny
HWND hWnd=NULL;         // uchwyt okna
HINSTANCE hInstance;    // instancja aplikacji

bool keys[256];         // Stan klawiszy
bool active=true;       // flaga zminimalizowania, domyslnie na tre
bool fullscreen=true;   // tryb pelnoekranowy, domyslnie na true

const CHAR nazwaAplikacji[] = "OpenGL_lab3d"; // nazwa klasy okna
const CHAR *nazwaTekstury[] = { "podloga.bmp", "sciana.bmp", "sufit.bmp", "sciana_exit.bmp" };

GLuint texture[ILOSC_TEKSTUR]; // tablica na nazwy tekstur

plansza plansza_; // serce aplikacji

// zmienne okreslajace czy klawisz zostal wcisniety
bool np; // nacisniety klawisz N
bool fp; // F
bool lp; // L

// kolor mgly
GLfloat fogColor[]= {0.3f, 0.3f, 0.3f, 1.0f};

// wartosci swiatla rozproszonego
GLfloat LightDiffuse[]= { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat LightAmbient[]= { 0.9f, 0.9f, 0.9f, 1.0f }; // wartosci swiatla otoczenia

// zmienna okresla czy chcemy miec mgle w labiryntcie
bool trybMgly=true;

// zmienna okresla czy chcemy miec swiatlo w labiryncie
bool swiatlo=true;


/*******************************************************************************
 * deklaracje funkcji
 *******************************************************************************
 */
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM); // procedura obslugi okna

// funkcja odpowiedzialna za zmiane rozmiarow okna
// ustawia perspektywe do rozmiarow okna po zmianie jego wymiarow
GLvoid ReSizeGLScene(GLsizei width, GLsizei height);

// funkcja ustawia opengl
int InitGL(GLvoid);

// funkcja rysujaca scene
int DrawGLScene(GLvoid);

// funkcja zwalnia pamiec z kontekstu renderowania, kontekstu urzadzenia i uchwyt okna
GLvoid KillGLWindow(GLvoid);

// funkcja tworzy okienko opengl
// argumenty: tytul, szerokosc, wysokosc, glebia kolorow (16/24/32), tryb pelnoekranowy
bool CreateGLWindow(char* title, int width, int height, int bits, bool fullscreenflag);

// wczytanie tekstury z pliku
// jezeli nie mozna wczytac pliku zwroc NULL
AUX_RGBImageRec *LoadBMP(const char *Filename);

// wczytuje bitmape i konwertuje ja na teksture
int LoadGLTextures();

// Stw�rz listy wy�wietlania
GLvoid BuildLists(); 


/*******************************************************************************
 * WIN MAIN
 *******************************************************************************
 */
int WINAPI WinMain(    
        HINSTANCE    hInstance,         // Instancja
        HINSTANCE    hPrevInstance,         // Poprzednia instancja
        LPSTR        lpCmdLine,         // Parametry z linii komend
        int    nCmdShow)         // Stan okna
{
    MSG    msg;         // Struktura przechowuj�ca komunikaty okna
    BOOL done=FALSE;         // Stan dzia�ania programu
    
    // Stw�rz okno OpenGL
    if (!CreateGLWindow("Labirynt 3D",1024,768,32,fullscreen))
    {
        return 0;         // zako�cz program
    } 
    
    // glowna petla programu
    while(!done){
                 if(PeekMessage(&msg,NULL,0,0,PM_REMOVE))         // czy s� jakie� wiadomo�ci ?
                      {
                        if(msg.message==WM_QUIT)         // czy otrzymano wiadomo�� zamkni�cia ?
                        {
                              done=TRUE;         // skoro tak, to done=TRUE
                        }
                        else         // nie otrzymano wiadomo�ci zamkni�cia ?
                        {
                              TranslateMessage(&msg);         // wyt�umacz wiadomo��
                              DispatchMessage(&msg);         // wy�lij j�       
                        }                                           
                      }
                      else{
                           // Rysuj scen� OpenGL
                           if(active)         // program jest aktywny ?
                           {
                                if(keys[VK_ESCAPE])         // czy wci�ni�ty jest ESC ?
                                {
                                     done=TRUE;         // przerwanie warunku p�tli
                                }
                                else         // nie ma czasu na zamkni�cie, rysujemy scene
                                {
                                     DrawGLScene();         // Rysuj scen�
                                     SwapBuffers(hDC);         // Zamie� bufory (ekrany)

									 if(keys[VK_LEFT]){
										 plansza_.obroc(-1.5f);
									 }

									 if(keys[VK_RIGHT]){
										 plansza_.obroc(1.5f);
									 }

									 if(keys[VK_UP]){
										 plansza_.ruch_UP();
									 }

									 if(keys[VK_DOWN]){
										 plansza_.ruch_DOWN();
									 }

									 if(keys['N'] && !np){
										np=true;
										plansza_.reset();
									 }

									 if(!keys['N']){
										np=false;
									 }

									 if(keys['F'] && !fp){
										fp=true;
										trybMgly=!trybMgly;
										if(trybMgly)
											glEnable(GL_FOG);
										else
											glDisable(GL_FOG);
									 }

									 if(!keys['F']){
										fp=false;
									 }

									 if(keys['L'] && !lp){
										lp=true;
										swiatlo=!swiatlo;
										if(swiatlo)
											glEnable(GL_LIGHTING);
										else
											glDisable(GL_LIGHTING);
									 }

									 if(!keys['L']){
										lp=false;
									 }
                                } 
                            } // koniec if active
                     } // koniec else peekmessage
                 } // koniec glownej petli programu
                 
    KillGLWindow();         // Zamknij OpenGL
    return (msg.wParam);         // Koniec programu
} 


/*******************************************************************************
 * definicje funkcji
 *******************************************************************************
 */
GLvoid ReSizeGLScene(GLsizei width, GLsizei height) {
    if(height==0)         // zapobiegnij dzieleniu przez zero...
    {
        height=1;         // ...ustawiaj�c liczb� 1
    }
    glViewport(0, 0, width, height);         // zresetuj pole widzenia       
    glMatrixMode(GL_PROJECTION);         // wybierz macierz projekcji
    glLoadIdentity();         // zresetuj j�
        // oblicz perspektyw� dla okna
    gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,20.0f);
    glMatrixMode(GL_MODELVIEW);         // wybierz macier modeli
    glLoadIdentity();         // zresetuj j� 
}

int InitGL(GLvoid){
	if (!LoadGLTextures())							// Jump To Texture Loading Routine
	{
		return FALSE;							// If Texture Didn't Load Return FALSE
	}

	glEnable(GL_TEXTURE_2D);						// Enable Texture Mapping

	BuildLists(); // zbuduj listy wyswietlania

    glShadeModel(GL_SMOOTH); // wlaczenie gladkiego cieniowania
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f); // ustawienie koloru czyszczenia na kolor mgly
    glClearDepth(1.0f);         // ustawienie bufora g�ebi
    glEnable(GL_DEPTH_TEST);         // wlaczenie testowania g��bi
    glDepthFunc(GL_LEQUAL);         // ustawienie typu testowania
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); // najlepsza perspektywa

	// mg�a
	glFogi(GL_FOG_MODE, GL_LINEAR);         // Tryb mg�y
    glFogfv(GL_FOG_COLOR, fogColor);         // Ustawienie koloru mg�y
    glFogf(GL_FOG_DENSITY, 0.35f);         // Jak g�sta b�dzie nasza mg�a
    glHint(GL_FOG_HINT, GL_DONT_CARE);         // Spos�b tworzenia (pikselowy albo wierzcho�kowy)
    glFogf(GL_FOG_START, 4.0f);         // Odleg�o�� pocz�tku mg�y
    glFogf(GL_FOG_END, 10.0f);         // Odleg�o�� ko�ca mg�y
    glEnable(GL_FOG);         // W��cza GL_FOG

	// swiatlo
	glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);
	glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);

	glEnable(GL_LIGHT1); // wlaczenie swiatel
	glEnable(GL_LIGHTING);

    return TRUE;
}

int DrawGLScene(GLvoid){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);         // czy�� ekran
    glLoadIdentity();         // zresetuj macierz modeli

	//glTranslatef(0.0f,0.0f,-10);
	//glRotatef(35, 1.0f, 0.0f, 0.0f);
	plansza_.rysuj();

    return TRUE;         // wszystko jest ok
}

GLvoid KillGLWindow(GLvoid){
       // czy program jest w trybie pelnoekranowym
       if(fullscreen){
                      ChangeDisplaySettings(NULL,0);         // przywr�c rozdzielczo��
                      ShowCursor(TRUE);         // poka� kursor
                      }
                      
       // czy posiadamy kontekst renderowania
       if(hRC){
               // sprawdzenie czy mozna go zwolnic
               if (!wglMakeCurrent(NULL,NULL))
                 {
                       MessageBox(NULL,"Nie mo�na zwolni� hDC lub hRC!","B��D ZAMYKANIA",MB_OK | MB_ICONINFORMATION);                                
                  }
               
               // proba usuniecia kontekstu renderowania
               if(!wglDeleteContext(hRC)){
                      MessageBox(NULL,"Nie mo�na usun�� kontekstu renderowania","B��D ZAMYKANIA",MB_OK | MB_ICONINFORMATION);                    
               }
               
               hRC=NULL;
       }
       
       // proba zwolnienia kontekstu urzadzenia
       if(hDC && !ReleaseDC(hWnd, hDC)){
              MessageBox(NULL,"Nie mo�na zwolni� kontekstu urz�dzenia (DC)","B��D ZAMYKANIA",MB_OK | MB_ICONINFORMATION);
              hDC=NULL;         // Ustawiamy DC na NULL
       }
       
       // proba zniszczenia uchwytu okna
       if(hWnd && !DestroyWindow(hWnd)){
               MessageBox(NULL,"Nie mo�na zwolni� hWnd","B��D ZAMYKANIA",MB_OK | MB_ICONINFORMATION);
               hWnd=NULL;         // Ustaw hWnd na Null        
       }
       
       // zwolnienie klasy okna
       if(!UnregisterClass(nazwaAplikacji, hInstance)){
                MessageBox(NULL,"Nie mo�na wyrejstrowa� klasy okna","B��D ZAMYKANIA",MB_OK | MB_ICONINFORMATION);
                hInstance=NULL;         // Ustawiamy instancj� na NULL                            
       }
}

bool CreateGLWindow(char* title, int width, int height, int bits, bool fullscreenflag)
{
     GLuint PixelFormat;         // B�dzie przechowywa� znaleziony format piksela
     WNDCLASS wc;         // Struktura klasy okna
     DWORD dwExStyle;         // Rozszerzony styl okna
     DWORD dwStyle;         // Normalny styl okna
     RECT WindowRect;         // Tu b�d� rozmiary okna
     WindowRect.left=(long)0;         // Pocz�tek szeroko�ci (od lewej) ma 0
     WindowRect.right=(long)width;         // Szeroko�c bierzemy z parametru naszej funkcji
     WindowRect.top=(long)0;         // Wysoko�� te� zaczynamy od 0 (od g�ry)
     WindowRect.bottom=(long)height;         // Ustawiamy wysoko�� z parametru naszej funkcji
     fullscreen=fullscreenflag;         // Ustawiamy globaln� zmienn�
     hInstance = GetModuleHandle(NULL);         // Pobieramy instancj� dla okna
     wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;         // Ustawiamy odmalowywanie
     wc.lpfnWndProc        = (WNDPROC) WndProc;         // WndProc b�dzie obs�ugiwa� komunikaty
     wc.cbClsExtra        = 0;         // nie wa�ne
     wc.cbWndExtra        = 0;         // nie wa�ne
     wc.hInstance        = hInstance;         // Ustawiamy instancj�
     wc.hIcon        = LoadIcon(NULL, IDI_WINLOGO);         // Ikona domy�lna
     wc.hCursor        = LoadCursor(NULL, IDC_ARROW);         // Kursor - strza�ka
     wc.hbrBackground    = NULL;         // T�o nie jest wa�ne w OpenGL
     wc.lpszMenuName        = NULL;         // Nie chcemy menu
     wc.lpszClassName    = nazwaAplikacji;         // Nazwa klasy okna
     
     if (!RegisterClass(&wc))         // Spr�buj zarejstrowa� klas� okna
     {
        MessageBox(NULL,"Nie uda�o si� zarejstrowa� klasy okna","B��D",MB_OK|MB_ICONEXCLAMATION);
        return FALSE;         // zako�cz i zwr�� fa�sz.
     }
     
     if (fullscreen)         // Czy ma by� pe�ny ekran ?
     {
        DEVMODE dmScreenSettings;         // Tryb karty graficznej
        memset(&dmScreenSettings,0,sizeof(dmScreenSettings));         // Wyczy�� pami��
        // Ustaw rozmiar tej struktury
        dmScreenSettings.dmSize=sizeof(dmScreenSettings);
        dmScreenSettings.dmPelsWidth    = width;        // Wybie� ��dan� szeroko��
        dmScreenSettings.dmPelsHeight    = height;        // Wybierz ��dan� wysoko��
        dmScreenSettings.dmBitsPerPel    = bits;         // Wybierz g��bie kolor�w
        dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;
        
        // Spr�buj ustawi� pe�ny ekran. CDS_FULLSCREEN usuwa pasek start.
        if(ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
        {
             MessageBox(NULL, "Nie uda�o si� ustawi� trybu pe�noekranowego", "B��D", MB_OK|MB_ICONEXCLAMATION);                                                                              
             return FALSE;
        }
        
        dwExStyle=WS_EX_APPWINDOW;         // Rozszerzony styl okna
        dwStyle=WS_POPUP;         // Styl okna
        ShowCursor(FALSE);         // Ukryj kursor myszy                    
     }else{
           dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;         // Rozszerzony styl okna
           dwStyle=WS_OVERLAPPEDWINDOW;         // Styl okna
           }
           
     AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);
     
     // tworzenie okienka
     if( !(hWnd=CreateWindowEx(    dwExStyle,         // Rozszerzony styl dla okna
            nazwaAplikacji,         // Nazwa klasy
            title,         // Tytu� okna
            WS_CLIPSIBLINGS |         // Wymagane style okna
            WS_CLIPCHILDREN |         // Wymagane style okna
            dwStyle,         // Wybrane style okna
            0, 0,         // Pozycja okna
            WindowRect.right-WindowRect.left,         // Szeroko��
            WindowRect.bottom-WindowRect.top,         // Wysoko��
            NULL,         // Nie u�ywamy okna potomnego
            NULL,         // �adnego menu
            hInstance,         // Instancja
            NULL)))         // Nie dawaj nic do WM_CREATE  
     {
        KillGLWindow();         // Zresetuj tryb ekranu
        MessageBox(NULL,"Nie mo�na stworzy� okna.","B��D",MB_OK|MB_ICONEXCLAMATION);
        return FALSE;         // zwr�� false
    }
    
    PIXELFORMATDESCRIPTOR pfd =         // pfd m�wi oknu co chcemy
    {
        sizeof(PIXELFORMATDESCRIPTOR),         // Rozmiar opisu piksela
        1,         // Numer wersji
        PFD_DRAW_TO_WINDOW |         // Format musi obs�ugiwa� okno
        PFD_SUPPORT_OPENGL |         // Format musi obs�ugiwa� OpenGL
        PFD_DOUBLEBUFFER,         // Musi obs�ugiwa� Podw�jne buforowanie
        PFD_TYPE_RGBA,         // i format RGBA
        bits,         // Wybieramy g��bie kolor�w
        0, 0, 0, 0, 0, 0,         // ignorujemy
        0,         // Bez bufora alpha
        0,         // Bit ignorujemy
        0,         // ignorujemy
        0, 0, 0, 0,         // ignorujemy
        16,         // 16 bitowy bufor Z
        0,         // ignorujemy
        0,         // ignorujemy
        PFD_MAIN_PLANE,         // G��wna warstwa rysowania
        0,         // zarezerwowane
        0, 0, 0         // ignorujemy maski warstw
    };
    
    // proba pobrania kontekstu urzadzenia
    if (!(hDC=GetDC(hWnd)))         // Mamy kontekst urz�dzenia?
    {
        KillGLWindow();         // Resetujemy ekran
        MessageBox(NULL,"Nie mo�na stworzy� kontekstu urz�dzenia.","B��D",MB_OK|MB_ICONEXCLAMATION);
        return FALSE;         // zwracamy false
    }
    
    if (!(PixelFormat=ChoosePixelFormat(hDC,&pfd)))         // Czy windows znajdzie taki format pixela?
    {
        KillGLWindow();         // Resetujemy ekran
        MessageBox(NULL,"Nie mo�na znale�� ��danego formatu piksela.","B��D",MB_OK|MB_ICONEXCLAMATION);
        return FALSE;         // zwracamy false
    }
    
    if (!SetPixelFormat(hDC,PixelFormat,&pfd))         // Czy mo�emy ustawi� taki format
    {
        KillGLWindow();         // Resetujemy ekran
        MessageBox(NULL,"Nie mo�na ustawi� ��danego formatu piksela.","B��D",MB_OK|MB_ICONEXCLAMATION);
        return FALSE;         // zwracamy false
    }
    
    if (!(hRC=wglCreateContext(hDC)))         // Czy mo�emy pobra� hRC
    {
        KillGLWindow();         // Resetujemy ekran
        MessageBox(NULL,"Nie mo�na stworzy� kontekstu renderowania.","B��D",MB_OK|MB_ICONEXCLAMATION);
        return FALSE;         // zwracamy false
    }
    
    if (!wglMakeCurrent(hDC,hRC))         // Czy mo�emy aktywowa� kontekst renderowania?
    {
        KillGLWindow();         // Resetujemy ekran
        MessageBox(NULL,"Nie mo�na aktywowa� kontekstu renderowania.","B��D",MB_OK|MB_ICONEXCLAMATION);
        return FALSE;         // zwracamy false
    }
    
    ShowWindow(hWnd,SW_SHOW);         // Pokazujemy okno
        // Ustawiamy wy�szy priorytet
    SetFocus(hWnd);         // Dzia�anie klawiatury skierowujemy na okno
    ReSizeGLScene(width, height);         // Ustawiamy perspektyw�
    
    if (!InitGL())         // Czy GL zanicjowa� si� ?
    {
        KillGLWindow();         // Resetujemy ekran
        MessageBox(NULL,"Inicjacja niepomy�lna.","B��D",MB_OK|MB_ICONEXCLAMATION);
        return FALSE;         // zwracamy false
    }
    
    return TRUE;       
}

LRESULT CALLBACK WndProc(    
    HWND    hWnd,         // uchwyt do okna
    UINT    uMsg,         // Wiadomo�� dla okna
    WPARAM    wParam,         // Dodatkowe informajce wiadomo�ci
    LPARAM    lParam)         // Dodatkowe informajce wiadomo�ci
{ 
    switch(uMsg){
                 case WM_ACTIVATE:         // Czy to wiadomo�� aktywowania?
                    if (!HIWORD(wParam))         // Czy program jest aktywowany
                    {
                        active=TRUE;         // Program jest aktywny
                    }
                    else
                    {
                        active=FALSE;         // Program nie jest aktywny
                    }
                    return 0;         // Powr�� do p�tli wiadomo�ci
                 
                 case WM_SYSCOMMAND:         // Czy to komenda systemowa?
                    switch (wParam)         // Sprawdzimy typ
                    {
                            case SC_SCREENSAVE:         // Zgaszacz ekranu chce si� w��czy�
                            case SC_MONITORPOWER:         // Monitor chce si� wy��czy�
                            return 0;         // Anulujemy wygaszacze itp.
                    }
                    break;         // koniec
                 
                 case WM_CLOSE:         // Czy to rozkaz zamkni�cia?
                      PostQuitMessage(0);         // Wy�lij wiadomo�� zamkni�cia
                      return 0;         // skocz dalej
             
                 case WM_KEYDOWN:         // Czy klawisz zosta� wci�ni�ty
                      keys[wParam]=TRUE;         // Odpowiednie pole zostaje ustawione
                      return 0;         // skocz dalej
        
                 case WM_KEYUP:         // Czy klawisz zosta� wci�ni�ty
                      keys[wParam]=FALSE;         // Odpowiednie pole zostaje ustawione na false
                      return 0;         // skocz dalej
                      
                 case WM_SIZE:         // Czy okno si� zmieni�o ?
                    ReSizeGLScene(LOWORD(lParam),HIWORD(lParam));         // Zmieniamy scene OpenGL
                    return 0;         // skocz dalej
         
                 }
     
     // inne wiadosci obslugiwane przez windows
     return DefWindowProc(hWnd,uMsg,wParam,lParam);   
}

AUX_RGBImageRec *LoadBMP(const char *Filename){
      FILE *File=NULL; // uchwyt pliku
      
      if (!Filename) // sprawdzenie czy nazwa pliku zostala podana
	  {
		return NULL; // jesli nie zwroc NULL
	  }

      // sprawdz czy podany plik istnieje
      File=fopen(Filename, "r");
      
      if(File){ // jezeli plik istnieje
               fclose(File); // zamkniecie go
               return auxDIBImageLoad(Filename);
               }
               
      return NULL;
}

int LoadGLTextures(){
    int Status=FALSE; // status ladowania tekstury
    
    AUX_RGBImageRec *TextureImage[ILOSC_TEKSTUR]; // miejsce na teksture
    
    memset(TextureImage,0,sizeof(void *)*ILOSC_TEKSTUR); // wyzerowanie wskaznika na to miejsce

	if(  (TextureImage[TEKSTURA_PODLOGA]=LoadBMP(nazwaTekstury[TEKSTURA_PODLOGA]))
		&& (TextureImage[TEKSTURA_SCIANA]=LoadBMP(nazwaTekstury[TEKSTURA_SCIANA]))
		&& (TextureImage[TEKSTURA_SUFIT]=LoadBMP(nazwaTekstury[TEKSTURA_SUFIT]))
		&& (TextureImage[TEKSTURA_SCIANA_EXIT]=LoadBMP(nazwaTekstury[TEKSTURA_SCIANA_EXIT]))
		){
			Status=TRUE;

			glGenTextures(ILOSC_TEKSTUR, texture); // utworzenie nazw dla  tekstur

			for(int x=0;x<ILOSC_TEKSTUR;x++){
				glBindTexture(GL_TEXTURE_2D, texture[x]);
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);
				gluBuild2DMipmaps(GL_TEXTURE_2D, 3, TextureImage[x]->sizeX, TextureImage[x]->sizeY, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[x]->data);
			}

	} // koniec if
	
	// ewentualne zwolnienie pamieci po teksturze
	for(int x=0;x<ILOSC_TEKSTUR;x++){
		if (TextureImage[x])							// If Texture Exists
		{
			if (TextureImage[x]->data)					// If Texture Image Exists
			{
				free(TextureImage[x]->data);				// Free The Texture Image Memory
			}

			free(TextureImage[x]);						// Free The Image Structure
		}
	}

	return Status;
}

GLvoid BuildLists(){
	pole_mur=glGenLists(ILOSC_LIST_WYSWIETLANIA); // przydziel miejsce na listy wyswietlania

	glNewList(pole_mur,GL_COMPILE); // pierwsza lista wyswitlania pole mur

		glBindTexture(GL_TEXTURE_2D, texture[TEKSTURA_SCIANA]);

		glBegin(GL_QUADS);
			glNormal3f(0.0f, 0.0f, 1.0f);
			// Front Face
			glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);	// Bottom Left Of The Texture and Quad
			glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);	// Bottom Right Of The Texture and Quad
			glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);	// Top Right Of The Texture and Quad
			glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);	// Top Left Of The Texture and Quad
			// Back Face
			glNormal3f(0.0f, 0.0f, -1.0f);
			glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);	// Bottom Right Of The Texture and Quad
			glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);	// Top Right Of The Texture and Quad
			glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);	// Top Left Of The Texture and Quad
			glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);	// Bottom Left Of The Texture and Quad
			// Right face
			glNormal3f(1.0f, 0.0f, 0.0f);
			glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);	// Bottom Right Of The Texture and Quad
			glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);	// Top Right Of The Texture and Quad
			glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);	// Top Left Of The Texture and Quad
			glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);	// Bottom Left Of The Texture and Quad
			// Left Face
			glNormal3f(-1.0f, 0.0f, 0.0f);
			glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);	// Bottom Left Of The Texture and Quad
			glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);	// Bottom Right Of The Texture and Quad
			glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);	// Top Right Of The Texture and Quad
			glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);	// Top Left Of The Texture and Quad
		glEnd();

	glEndList();

	pole_puste=pole_mur+1;

	glNewList(pole_puste, GL_COMPILE);
		glBindTexture(GL_TEXTURE_2D, texture[TEKSTURA_SUFIT]);

		glBegin(GL_QUADS);

			// Top Face
			glNormal3f(0.0f, -1.0f, 0.0f);
			glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
			glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
			glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
			glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
	
		glEnd();

		glBindTexture(GL_TEXTURE_2D, texture[TEKSTURA_PODLOGA]);

		glBegin(GL_QUADS);
			// Bottom Face
			glNormal3f(0.0f, 1.0f, 0.0f);
			glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, -1.0f, -1.0f);	// Top Right Of The Texture and Quad
			glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f, -1.0f, -1.0f);	// Top Left Of The Texture and Quad
			glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);	// Bottom Left Of The Texture and Quad
			glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);	// Bottom Right Of The Texture and Quad
		glEnd();

	glEndList();

	sciana_N=pole_puste+1; // sciana N

	glNewList(sciana_N, GL_COMPILE);
		glBindTexture(GL_TEXTURE_2D, texture[TEKSTURA_SCIANA]);
		glBegin(GL_QUADS);
			// Back Face
			glNormal3f(0.0f, 0.0f, 1.0f);
			glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);	// Bottom Right Of The Texture and Quad
			glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);	// Top Right Of The Texture and Quad
			glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);	// Top Left Of The Texture and Quad
			glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);	// Bottom Left Of The Texture and Quad
		glEnd();
	glEndList();

	sciana_S=sciana_N+1; // sciana S

	glNewList(sciana_S, GL_COMPILE);
		glBindTexture(GL_TEXTURE_2D, texture[TEKSTURA_SCIANA]);
		glBegin(GL_QUADS);
			glNormal3f(0.0f, 0.0f, -1.0f);
			// Front Face
			glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);	// Bottom Left Of The Texture and Quad
			glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);	// Bottom Right Of The Texture and Quad
			glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);	// Top Right Of The Texture and Quad
			glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);	// Top Left Of The Texture and Quad
		glEnd();
	glEndList();

	sciana_W=sciana_S+1; // sciana W
	
	glNewList(sciana_W, GL_COMPILE);
		glBindTexture(GL_TEXTURE_2D, texture[TEKSTURA_SCIANA]);
		glBegin(GL_QUADS);
			// Left Face
			glNormal3f(1.0f, 0.0f, 0.0f);
			glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);	// Bottom Left Of The Texture and Quad
			glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);	// Bottom Right Of The Texture and Quad
			glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);	// Top Right Of The Texture and Quad
			glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);	// Top Left Of The Texture and Quad
		glEnd();
	glEndList();

	sciana_E=sciana_W+1; // sciana E
	
	glNewList(sciana_E, GL_COMPILE);
		glBindTexture(GL_TEXTURE_2D, texture[TEKSTURA_SCIANA]);
		glBegin(GL_QUADS);
			// Right face
			glNormal3f(-1.0f, 0.0f, 0.0f);
			glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);	// Bottom Right Of The Texture and Quad
			glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);	// Top Right Of The Texture and Quad
			glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);	// Top Left Of The Texture and Quad
			glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);	// Bottom Left Of The Texture and Quad
		glEnd();
	glEndList();

	sciana_N_exit=sciana_E+1; // sciana N exit

	glNewList(sciana_N_exit, GL_COMPILE);
		glBindTexture(GL_TEXTURE_2D, texture[TEKSTURA_SCIANA_EXIT]);
		glBegin(GL_QUADS);
			// Back Face
			glNormal3f(0.0f, 0.0f, 1.0f);
			glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);	// Bottom Right Of The Texture and Quad
			glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);	// Top Right Of The Texture and Quad
			glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);	// Top Left Of The Texture and Quad
			glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);	// Bottom Left Of The Texture and Quad
		glEnd();
		glEnd();
	glEndList();

	sciana_W_exit=sciana_N_exit+1;

	glNewList(sciana_W_exit, GL_COMPILE);
		glBindTexture(GL_TEXTURE_2D, texture[TEKSTURA_SCIANA_EXIT]);
		glBegin(GL_QUADS);
			// Left Face
			glNormal3f(1.0f, 0.0f, 0.0f);
			glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);	// Bottom Left Of The Texture and Quad
			glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);	// Bottom Right Of The Texture and Quad
			glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);	// Top Right Of The Texture and Quad
			glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);	// Top Left Of The Texture and Quad
		glEnd();
	glEndList();
}