#define _CRT_SECURE_NO_WARNINGS 

#include <stdlib.h>
#include <stdio.h>
#include <iostream>

struct Rotor {
	short* perm;						//tablica do przechowywania jego permutacji
	short notch_n;						//ilosc liter powodujacych obrot innych wirnikow
	short* notch;						//tablica do przechowywania tych liter
	short t_n;							//liczba obrotow, ktore wykonal rotor od swojego pierwotnego polozenia
};

struct Reflector {
	short* perm;						//tablica do przechowywania jego permutacji
};

struct Machine {
	short n;							//wielkosc alfabetu
	short m;							//liczba wirnikow
	short l;							//liczba reflektorow
	Rotor* rotors;						//tablica przechowujaca rotory
	Reflector* reflectors;				//tablica przechowujaca reflektory
};

struct Task {		
	short p;							//liczba zadan do rozwiazania
	short ro_n;							//liczba obcenie uzywanych rotorow
	short* ro_id;						//tablica indeksow obecnie uzywanych rotorow
	short* s_pos;						//tablica pozycji startowa rotora
	short re_n;							//indeks obecnie uzywanego reflektora
	short msg_size;						//rozmiar tablicy, ktora zawiera zdanie do zaszyfrowania
	short* msg;							//tablica do przechowywania wiadomosci do zakodowania
};

//przesuwa KOPIE ROTORA na pozycje startowa okreslona w konfiguracji zadan
short* rotate_to_s_pos(short n, short* perm,short s_pos)
{
	short* pom = (short*)malloc(n * sizeof(short));
	for (int i = 0; i < n; i++)
	{
		pom[i] = perm[(i + s_pos - 1) % n];
	}
	return pom;
}

short* rotate(short n, short* perm)
{
	short* pom = (short*)malloc(n * sizeof(short));
	for (int i = 0; i < n; i++)
	{
		pom[i] = perm[(i + 1) % n];
	}
	return pom;
}

//zwraca indeks liczby, ktora trzeba sprawdzic w kolejnym rotorze lub reflektorze
//input - indeks liczby, ktora sie sprawdza
//output - indeks liczby, ktora bedzie sprawdzana w kolejnym rotorze/reflektorze
short return_index(short input, Rotor r, short n)
{
	input--;										//przez to bede mial tutaj taka liczbe, jakiego indeksu dokladnie szukam				
													//zapisuje liczbe, ktorej odpowiadajacej liczby z a podst bede szukal

	for (int i = 0; i < n; i++)
	{
		if (r.perm[input] - 1 == (i + r.t_n) % n)
		{

			return i + 1;
		}
	}
	return 0;
}

short return_index_r(short input, Rotor r, short n)
{							
	input = (input + r.t_n) % n;

	for (int i = 0; i < n; i++)
	{
		if (input == r.perm[i]%n)
		{
			return i + 1;
		}
	}
	return 0;
}

//input - indeks liczby, ktora wchodzi do reflektora						
//output - indeks, pod ktorym trzeba sprawdzic alfabet podstawowy kolejnego wirnika
short return_index(short input, Reflector r)
{
	return r.perm[input - 1];
}

void machine_config(Machine& machine)
{
	scanf("%hu", &machine.n);
	scanf("%hu", &machine.m);
	machine.rotors = (Rotor*)malloc(machine.m * sizeof(Rotor));

	short j = 0;
	short x = 0;
	for (int i = 0; i < machine.m; i++)
	{
		j = 0;
		machine.rotors[i].t_n = 0;
		machine.rotors[i].perm = (short*)malloc(machine.n * sizeof(short));

		while (j < machine.n )
		{
			scanf("%hu ", &machine.rotors[i].perm[j]);
			j++;
		}
		scanf("%hu", &machine.rotors[i].notch_n);
		if (machine.rotors[i].notch_n)
		{
			machine.rotors[i].notch = (short*)malloc(machine.rotors[i].notch_n * sizeof(short));
			x = 0;
			while (x < machine.rotors[i].notch_n)
			{
				scanf("%hu", &machine.rotors[i].notch[x]);
				x++;
			}
		}
	}

	scanf("%hu", &machine.l);
	machine.reflectors = (Reflector*)malloc(machine.l * sizeof(Reflector));
	for (int i = 0; i < machine.l; i++)
	{
		machine.reflectors[i].perm = (short*)malloc(machine.n * sizeof(short)); // czy to "int" jest potrzebne?
		j = 0;
		while (j < machine.n)
		{
			scanf("%hu ", &machine.reflectors[i].perm[j]);
			j++;
		}
	}
}

void generate_cipher(Task& t, Machine machine)
{
	Rotor* rotors = (Rotor*)malloc(t.ro_n * sizeof(Rotor));					//to jest chyba najwieksze gowno w calym kodzie
	//przygotowanie rotorow - obracanie ich na pozycje startowe
	for (int x = 0; x < t.ro_n; x++)
	{
		rotors[x].perm = rotate_to_s_pos(machine.n, machine.rotors[t.ro_id[x]].perm, t.s_pos[x]);
		rotors[x].t_n = t.s_pos[x] - 1;
		rotors[x].notch_n = machine.rotors[t.ro_id[x]].notch_n;
		rotors[x].notch = machine.rotors[t.ro_id[x]].notch;
	}

	//kodowanie wlasciwej wiadomosci
	for (int i = 0; i < t.msg_size; i++)
	{

		for (int j = 0; j < t.ro_n; j++)
		{
			if (j == 0)
			{
				rotors[j].perm = rotate(machine.n, rotors[j].perm);				//tutaj krzyczy access violation xD
				rotors[j].t_n++;
			}
			t.msg[i] = return_index(t.msg[i], rotors[j], machine.n);

			if (rotors[j].notch_n && j < t.ro_n - 1)
			{
				for (int o = 0; o < rotors[j].notch_n; o++)
				{
					if (rotors[j].notch[o] == rotors[j].perm[0])
					{
						rotors[j + 1].perm = rotate(machine.n, rotors[j + 1].perm);
						rotors[j + 1].t_n++;
					}
				}
			}
		}
		t.msg[i] = return_index(t.msg[i], machine.reflectors[t.re_n]);
		for (int j = t.ro_n - 1; j >= 0; j--)
		{
			t.msg[i] = return_index_r(t.msg[i], rotors[j], machine.n);
		}
		printf("%hu ", t.msg[i]);
	}
	printf("\n");
	free(rotors);
	rotors = NULL;
}

void tasks(Machine& machine)
{
	Task* t = (Task*)malloc(sizeof(Task));
	scanf("%hu", &t->p);
	short x = 0;
	for (int i = 0; i < t->p; i++)
	{
		t->msg_size = 2;
		scanf("%hu ", &t->ro_n);
		t->ro_id = (short*)malloc(t->ro_n * sizeof(short));
		t->s_pos = (short*)malloc(t->ro_n * sizeof(short));

		for (int j = 0; j < t->ro_n; j++)
		{
			scanf("%hu ", &t->ro_id[j]);
			scanf("%hu ", &t->s_pos[j]);
		}
		scanf("%hu ", &t->re_n);

		t->msg = (short*)malloc(t->msg_size * sizeof(short));
		
		x = 0;

		while (std::cin >> t->msg[x])
		{
			if (t->msg[x] == 0)
			{
				break;
			}
			else
			{
				x++;
			}
			if (x == t->msg_size)
			{
				t->msg_size++;
				t->msg = (short*)realloc(t->msg, t->msg_size * sizeof(short));
			}
		}

		t->msg_size = x;
		generate_cipher(*t, machine);
		free(t->msg);
		free(t->ro_id);
		free(t->s_pos);
		t->msg = NULL;
		t->ro_id = NULL;
		t->s_pos = NULL;
	}
	free(t);
	t = NULL;
}

int main()
{
	Machine machine;
	machine_config(machine);
	tasks(machine);

	return 0;
}