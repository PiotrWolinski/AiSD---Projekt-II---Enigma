#define _CRT_SECURE_NO_WARNINGS 

#include <stdlib.h>
#include <stdio.h>
#include <iostream>

struct Rotor {
	unsigned int n=0;				 //wielkosc alfabetu
	unsigned int* perm;				 //tablica do przechowywania jego permutacji
	unsigned int notch_n=0;			 //ilosc liter powodujacych obrot innych wirnikow
	unsigned int* notch;			 //tablica do przechowywania tych liter
	unsigned int s_pos = 1;			 //pozycja startowa
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
	for (int x = 0; x < t.ro_n; x++)
	{
		rotors[x].perm = rotate_to_s_pos(machine.rotors[t.ro_id[x]].n, machine.rotors[t.ro_id[x]].perm, t.s_pos[x]);
	}
	
	for (int i = 0; i < t.ro_n; i++)
	{
		for (int j = 0; j < machine.n; j++)
		{

			printf("%u ", rotors[i].perm[j]);

		}

	}

	for (int i = 0; i < t.msg_size; i++)
	{
		/*t.msg[i] = */


	}



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