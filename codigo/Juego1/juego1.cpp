#include <stdio.h>
#include <thread>  
#include <chrono>
#include <conio.h>
#include <time.h>
#include <list>
using namespace std;

//Constantes
#define ANCHO_MUNDO 79
#define DERECHA 1
#define IZQUIERDA 0
#define INC_VELECIDAD 5

typedef unsigned int uint;

const double ESPERA_MILISEGS = 10;
const double MIL = 1000;

struct Personaje
{
    //variables personaje
    unsigned int m_posxPersonaje = ANCHO_MUNDO/2;
    unsigned char m_personaje_actual = '>';
    unsigned char m_piso_C = '_';
    unsigned char m_personaje_d = '>';
    unsigned char m_personaje_i = '<';
	bool m_direccion = DERECHA;

    Personaje(){};
};

struct Bala
{
    //variables bala
    unsigned int m_posxBala = 2;
    unsigned int m_velBala = 50;
    bool m_direccion_bala = DERECHA;
    bool m_balaActiva = false;
    unsigned char m_personaje_bala = 'o';
	double m_incrementoBala = 0.0;

    Bala(){};
};

struct Enemigo
{
    unsigned int m_posxEnem = 0;
	bool m_enemActivo = false;
	bool m_direccion_enemigo = IZQUIERDA;
	unsigned char m_personaje_enem = '|';
	double m_incrementoEnem = 0.0;
};

//variables generales
bool g_mover = false;
unsigned unsigned int random = 0;
Personaje *g_prota;
bool g_muerte = false;
unsigned int g_velEnem = 10; 
bool g_esSalida = false;
char g_key = ' ';
char *g_piso;
unsigned int g_score;
double g_frameRate = 0.0;

//Funciones
void ocultaEnemigos();
bool getInput(char *c);
void revisaColision(Enemigo *enem);
bool run();
bool draw();
void input();
void updateBalas();
void updateEnem();
void execute();
void creaPiso();
void init();
void cleanWorld();
void death();
void updateDifficulty();

static std::list<Bala*> balaList;
static std::list<Enemigo*> enemList;

int main()
{
	printf("\n\n\n");
	printf("PRACTICA 2 INGENIERIA DE SOFTWARE\n");
	printf("PARA JUGAR USAR LAS TECLAS:\n");
	printf("A MUEVE A LA IZQUIERDA\n");
	printf("B MUEVE A LA DERECHA\n");
	printf("D DISPARA\n");
	printf("PARA SALIR PRESIONA Q\n");
	printf("OBJETIVO DEL JUEGO:\n");
	printf("DEBES DESTRUIR A LOS ENEMIGOS QUE VAN SALIENDO A AMBOS LADOS DE LA PANTALLA\n");
	printf("CADA VEZ QUE DESTRUYES 2 ENEMIGOS LA VELOCIDAD AUMENTA\n");
	printf("\n\n\n");

	execute();
    return 0;
}

void execute()
{
	init();
	creaPiso();

	while (true && !g_esSalida)
	{
		if (!g_muerte) {
			run();
            if (g_esSalida)
                break;
			draw();
		}
		else
		{
			printf("Has muerto tu score es: %d Presiona barra para volver a jugar o q para salir", g_score);
			printf("\r");
			input();
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(ESPERA_MILISEGS)));
	}

}

void init()
{
	srand(time(NULL));
	g_frameRate = ESPERA_MILISEGS / MIL;
	g_prota = new Personaje();
	g_muerte = false;
	g_score = 0;
	g_velEnem = INC_VELECIDAD;
}

bool run()
{
	random = rand() % 100;
	input();
    if (!g_esSalida)
    {
        updateBalas();
        updateEnem();
        return false;
    }
    else
        return false;
}

bool draw()
{
    if (!g_muerte)
    {
        for (unsigned int x = 0; x < ANCHO_MUNDO; x++){
            g_piso[x] = '_';
        }
        for (unsigned int x = 0; x < ANCHO_MUNDO; x++){
            if (x == g_prota->m_posxPersonaje)
                g_piso[x] = g_prota->m_personaje_actual;
            //Draw Balas
            list<Bala*>::iterator i;
            for (i = balaList.begin(); i != balaList.end(); ++i)
            {
                Bala *bala = *i;
                if (bala->m_balaActiva) {
                    if (x == bala->m_posxBala)
                        g_piso[x] = bala->m_personaje_bala;
                }
            }
            //Draw Enemigos
            list<Enemigo*>::iterator iEnemigo;
            for (iEnemigo = enemList.begin(); iEnemigo != enemList.end(); ++iEnemigo)
            {
                Enemigo *enem = *iEnemigo;
                if (enem->m_enemActivo) {
                    if (x == enem->m_posxEnem)
                    {
                        g_piso[x] = enem->m_personaje_enem;
                        break;
                    }
                }
            }
        }

        printf("%s", g_piso);
        //printf("%d%d%s", g_posxBala, g_posxEnem, g_piso);
        printf("\r");
    }
    return false;
}

void input()
{
	if (getInput(&g_key))
	{
		if (g_key == 's')
		{
			g_prota-> m_direccion = DERECHA;
			g_prota->m_personaje_actual = g_prota->m_personaje_d;
			g_prota->m_posxPersonaje++;
		}
		if (g_key == 'a')
		{
			g_prota-> m_direccion = IZQUIERDA;
			g_prota->m_personaje_actual = g_prota->m_personaje_i;
			g_prota->m_posxPersonaje--;
		}
		if (g_key == 'd')
		{
			Bala *bala = new Bala();
			if (g_prota-> m_direccion == IZQUIERDA){
				bala->m_direccion_bala = IZQUIERDA;
			}
			else if (g_prota->m_direccion == DERECHA){
				bala->m_direccion_bala = DERECHA;
			}
			bala->m_balaActiva = true;
			bala->m_posxBala = g_prota->m_posxPersonaje;
			balaList.push_front(bala);
		}
		if (g_key == 32)
		{
			init();
		}
		if (g_key == 'q')
		{
            list<Bala*>::iterator iBala;
            for (iBala = balaList.begin(); iBala != balaList.end(); ++iBala)
            {
                Bala *bala = *iBala;
                delete bala;
            }

            balaList.clear();
            
            list<Enemigo*>::iterator iEnem;
            for (iEnem = enemList.begin(); iEnem != enemList.end(); ++iEnem)
            {
                Enemigo *enem = *iEnem;
                delete enem;
            }
            enemList.clear();
			g_esSalida = true;
            delete g_prota;
            return;
		}
	}
}

bool getInput(char *c)
{
    if (_kbhit())
    {
        *c = _getch();
        return true;
    }
    return false;
}

void revisaColision(Enemigo *enem)
{
    list<Bala*>::iterator i;

    for (i = balaList.begin(); i != balaList.end(); ++i)
    {
        Bala *bala = *i;
        unsigned int posBala = bala->m_posxBala;
        unsigned int posEnem = enem->m_posxEnem;
        if (bala->m_direccion_bala == DERECHA &&
            enem->m_direccion_enemigo == DERECHA &&
            bala->m_balaActiva &&
            enem->m_enemActivo)
        {
            if (posBala >= posEnem)
            {
                bala->m_balaActiva = false;
                enem->m_enemActivo = false;
                g_score++;
                updateDifficulty();
            }

        }

        if (bala->m_direccion_bala == IZQUIERDA && enem->m_direccion_enemigo == IZQUIERDA && bala->m_balaActiva && enem->m_enemActivo)
        {
            if (posBala <= posEnem)
            {
                bala->m_balaActiva = false;
                enem->m_enemActivo = false;
                g_score++;
                updateDifficulty();
            }
        }
    }
}


void updateEnem()
{
	if (random == 5 || random == 13)
	{
		Enemigo *enem = new Enemigo();
		//printf("%d",random);
		//Si random == 3 entonces el enemigo sale por la derecha
		enem->m_direccion_enemigo = (random == 5 ? DERECHA : IZQUIERDA);
		if (enem->m_direccion_enemigo == DERECHA)
		{
			enem->m_posxEnem = ANCHO_MUNDO;
		}
		else
		{
			enem->m_posxEnem = 0;
		}
		enem->m_enemActivo = true;
		enemList.push_front(enem);
	}
	list<Enemigo*>::iterator iEnem;
	for (iEnem = enemList.begin(); iEnem != enemList.end(); iEnem++)
	{
		Enemigo *enem = *iEnem;
		if (enem->m_enemActivo)
		{
			enem->m_incrementoEnem += g_frameRate*g_velEnem;

			if (enem->m_direccion_enemigo == DERECHA){
				if (enem->m_incrementoEnem >= 1)
				{
					enem->m_posxEnem -= 1;
					enem->m_incrementoEnem = 0;
					//Revisio si hay colision con el personaje
					if (enem->m_posxEnem < g_prota->m_posxPersonaje)
					{
						enem->m_enemActivo = false;
						death();
						return;
					}
				}

			}
			else {
				if (enem->m_incrementoEnem >= 1)
				{
					enem->m_posxEnem += 1;
					enem->m_incrementoEnem = 0;
					if (enem->m_posxEnem > g_prota->m_posxPersonaje)
					{
						enem->m_enemActivo = false;
						death();
						return;
					}
				}
			}
			revisaColision(enem);
		}
	}
}

void updateBalas()
{
	list<Bala*>::iterator i;
	for (i = balaList.begin(); i != balaList.end(); ++i)
	{
		Bala *bala = *i;

		if (bala->m_balaActiva)
		{
			bala->m_incrementoBala += g_frameRate*bala->m_velBala;
			if (bala->m_direccion_bala == DERECHA)
			{
				if (bala->m_incrementoBala >= 1)
				{
					bala->m_posxBala += 1;
					bala->m_incrementoBala = 0;
				}
			}
			else if (bala->m_direccion_bala == IZQUIERDA)
			{
				if (bala->m_incrementoBala >= 1)
				{
					bala->m_posxBala -= 1;
					bala->m_incrementoBala = 0;
				}
			}
			if (bala->m_posxBala > ANCHO_MUNDO || bala->m_posxBala < 0)
				bala->m_balaActiva = false;

		}
	}
}

void updateDifficulty()
{
	if (g_score % 2 == 0)
	{
        g_velEnem += INC_VELECIDAD;
	}
}

void creaPiso()
{
	g_piso = new char[ANCHO_MUNDO];
	for (int x = 0; x < ANCHO_MUNDO; x++){
		g_piso[x] = '_';
	}
	g_piso[ANCHO_MUNDO] = '\0';
}

void death()
{
	g_muerte = true;
	cleanWorld();
	balaList.clear();
	enemList.clear();
}

void cleanWorld()
{
	char *clean = new char[ANCHO_MUNDO];
	for (int x = 0; x < ANCHO_MUNDO; x++){
		clean[x] = ' ';
	}
	clean[ANCHO_MUNDO] = '\0';
	printf("%s", clean);
	printf("\r");
}
