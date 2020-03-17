#define _CRT_SECURE_NO_WARNINGS 

#include <stdlib.h>
#include <stdio.h>
#include <iostream>

struct Rotor {
	unsigned int n=0;				 //wielkosc alfabetu
	unsigned int* perm;				 //tablica do przechowywania jego permutacji
	unsigned int notch_n=0;			 //ilosc liter powodujacych obrot innych wirnikow
	unsigned int* notch;			 //tablica do przechowywania tych liter
	unsigned int t_n = 0;			 //liczba obrotow, ktore wykonal rotor od swojego pierwotnego polozenia
};

struct Reflector {
	unsigned int n = 0;				//wielkosc alfabetu
	unsigned int* perm;				//tablica do przechowywania jego permutacji
};

struct Machine {
	unsigned int n = 0;				//wielkosc alfabetu
	unsigned int m = 0;				//liczba wirnikow
	unsigned int l = 0;				//liczba reflektorow
	Rotor* rotors;					//tablica przechowujaca rotory
	Reflector* reflectors;			//tablica przechowujaca reflektory
};

struct Task {
	unsigned int p = 0;				//liczba zadan do rozwiazania
	unsigned int ro_n = 0;			//liczba obcenie uzywanych rotorow
	unsigned int* ro_id;			//tablica indeksow obecnie uzywanych rotorow
	unsigned int* s_pos;			//tablica pozycji startowa rotora
	unsigned int re_n = 0;			//indeks obecnie uzywanego reflektora
	unsigned int msg_size = 2;		//rozmiar tablicy, ktora zawiera zdanie do zaszyfrowania
	unsigned int* msg;				//tablica do przechowywania wiadomosci do zakodowania
};

//przesuwa KOPIE ROTORA na pozycje startowa okreslona w konfiguracji zadan
unsigned int* rotate_to_s_pos(unsigned int n, unsigned int* perm, int s_pos)
{
	unsigned int* pom = (unsigned int*)malloc(n * sizeof(unsigned int));
	for (int i = 0; i < n; i++)
	{
		pom[i] = perm[(i + s_pos - 1) % n];
	}
	return pom;
}

unsigned int* rotate_before(unsigned int n, unsigned int* perm)
{
	unsigned int* pom = (unsigned int*)malloc(n * sizeof(unsigned int));
	for (int i = 0; i < n; i++)
	{
		pom[i] = perm[(i + 1) % n];
	}
	return pom;
}

//zwraca indeks liczby, ktora trzeba sprawdzic w kolejnym rotorze lub reflektorze
//input - indeks liczby, ktora sie sprawdza
//output - indeks liczby, ktora bedzie sprawdzana w kolejnym rotorze/reflektorze
unsigned int return_index(unsigned int input, Rotor r)
{
	input--;									//przez to bede mial tutaj taka liczbe, jakiego indeksu dokladnie szukam
	unsigned int tmp = r.perm[input] - 1;		//zapisuje liczbe, ktorej odpowiadajacej liczby z a podst bede szukal
	unsigned int index = 0;

	for (int i = 0; i < r.n; i++)
	{
		if (tmp == (i + r.t_n) % r.n)
		{
			index = i;
			break;
		}
	}
	return index + 1;
}

unsigned int return_index_r(unsigned int input, Rotor r)
{							
	input = (input + r.t_n) % r.n;
	unsigned int index = 0;

	for (int i = 0; i < r.n; i++)
	{
		if (input == r.perm[i]%r.n)
		{
			index = i;
			break;
		}
	}
	return index + 1;
}

//input - indeks liczby, ktora wchodzi do reflektora						POPRAWIE, ZEBY NIE BYLO PETLI
//output - indeks, pod ktorym trzeba sprawdzic alfabet podstawowy kolejnego wirnika
unsigned int return_index(unsigned int input, Reflector r)
{
	input--;									//przez to bede mial tutaj taka liczbe, jakiego indeksu dokladnie szukam
	unsigned int tmp = r.perm[input] - 1;		//zapisuje liczbe, ktorej odpowiadajacej liczby z a podst bede szukal
	unsigned int index = 0;

	for (int i = 0; i < r.n; i++)
	{
		if (tmp == i)
		{
			index = i;
			break;
		}
	}
	return index + 1;
}

void machine_config(Machine& machine)
{
	scanf("%u", &machine.n);
	scanf("%u", &machine.m);
	machine.rotors = (Rotor*)malloc(machine.m * sizeof(Rotor));

	for (int i = 0; i < machine.m; i++)
	{
		machine.rotors[i].n = machine.n;
		machine.rotors[i].perm = (unsigned int*)malloc(machine.n * sizeof(unsigned int));
		int j = 0;
		while (j < machine.n )
		{
			std::cin >> machine.rotors[i].perm[j];
			j++;
		}
		scanf("%u", &machine.rotors[i].notch_n);
		if (machine.rotors[i].notch_n)
		{
			machine.rotors[i].notch = (unsigned int*)malloc(machine.rotors[i].notch_n * sizeof(unsigned int));
			int x = 0;
			while (x < machine.rotors[i].notch_n)
			{
				std::cin >> machine.rotors[i].notch[x];
				x++;
			}
		}
	}

	scanf("%u", &machine.l);
	machine.reflectors = (Reflector*)malloc(machine.l * sizeof(Reflector));
	for (int i = 0; i < machine.l; i++)
	{
		machine.reflectors[i].n = machine.n;
		machine.reflectors[i].perm = (unsigned int*)malloc(machine.n * sizeof(unsigned int));
		int j = 0;
		while (j < machine.n)
		{
			std::cin >> machine.reflectors[i].perm[j];
			j++;
		}
	}
}

void generate_cipher(Task t, Machine machine)
{
	Rotor* rotors = (Rotor*)malloc(t.ro_n * sizeof(Rotor));
	//przygotowanie rotorow - obracanie ich na pozycje startowe
	for (int x = 0; x < t.ro_n; x++)
	{
		rotors[x].perm = rotate_to_s_pos(machine.rotors[t.ro_id[x]].n, machine.rotors[t.ro_id[x]].perm, t.s_pos[x]);
		rotors[x].t_n = t.s_pos[x] - 1;
		rotors[x].n = machine.n;
	}

	//kodowanie wlasciwej wiadomosci
	for (int i = 0; i < t.msg_size; i++)
	{
		unsigned int tmp = t.msg[i];
		for (int j = 0; j < t.ro_n; j++)
		{
			if (!j)
			{
				rotors[j].perm = rotate_before(rotors[j].n, rotors[j].perm);
				rotors[j].t_n++;
			}
			tmp = return_index(tmp, rotors[j]); 
		}
		tmp = return_index(tmp, machine.reflectors[t.re_n]);
		for (int j = t.ro_n - 1; j >= 0; j--)
		{
			tmp = return_index_r(tmp, rotors[j]);
		}
		printf("%u ", tmp);			
	}
	printf("\n");
}

void tasks(Machine& machine)
{
	Task t;
	scanf("%u", &t.p);
	for (int i = 0; i < t.p; i++)
	{
		t.msg_size = 1;
		std::cin >> t.ro_n;
		t.ro_id = (unsigned int*)malloc(t.ro_n * sizeof(unsigned int));
		t.s_pos = (unsigned int*)malloc(t.ro_n * sizeof(unsigned int));
		for (int j = 0; j < t.ro_n; j++)
		{
			std::cin >> t.ro_id[j];
			std::cin >> t.s_pos[j];
		}
		std::cin >> t.re_n;

		t.msg = (unsigned int*)malloc(t.msg_size * sizeof(unsigned int)); //robie miejsce na dwie cyfry
		unsigned int x = 0;
		int tmp = 0;
		while (std::cin >> t.msg[x])
		{
			tmp = t.msg[x];
			if (t.msg[x] == 0)
			{
				break;
			}
			else
			{
				x++;
			}
			if (x == t.msg_size)
			{
				t.msg_size+=2;
				t.msg = (unsigned int*)realloc(t.msg, t.msg_size * sizeof(unsigned int));
			}
		}
		t.msg_size = x;
		generate_cipher(t, machine);
	}
	free(t.msg);
}

int main()
{
	Machine machine;
	machine_config(machine);
	tasks(machine);

	return 0;
}