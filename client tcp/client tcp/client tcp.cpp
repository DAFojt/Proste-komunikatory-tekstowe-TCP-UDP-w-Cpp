#include "stdafx.h"
#include <stdio.h>
#include <winsock2.h>
#include <string>
#include <string.h>
#include <conio.h>
#include <fstream>
#include <iostream>
#include <ws2tcpip.h>
#include <windows.h>
#include <stdio.h>

#pragma comment (lib, "ws2_32.lib")
using namespace std;
class WinSock
{
	WSADATA wsdata;
	SOCKET s;
	struct sockaddr_in local, remote; //adres,used by Windows Sockets to specify a local or remote endpoint address to which to connect a socket.

	int i = 0;
	int ile;

	public:
	WinSock()
	{
		inicjalizuj();
		gniazdo();
		binduj();
		polacz();
	}

	private:
	void inicjalizuj()
	{
		if (WSAStartup(MAKEWORD(2, 2), &wsdata) != 0) // Inicjalizacja Winsock
		{
			printf("Blad Inicjalizacji Winsock. Error: %d\n", WSAGetLastError());
			exit(EXIT_FAILURE);
		}
		else { printf("[OK] Inicjalizacja Winsock.\n"); }
	}
	void gniazdo()
	{
		if ((s = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) // stworzenie gniazda TCP = SOCK_STREAM
		{
			printf("Blad tworzenia socket : %d", WSAGetLastError());
		}
		else { printf("[OK] Tworzenie gniazda.\n"); }
		
		remote.sin_family = AF_INET;
		remote.sin_port = htons(8888);
		remote.sin_addr.s_addr = inet_addr("127.0.0.1");
		local.sin_family = AF_INET;
		local.sin_port = htons(27016);
		local.sin_addr.s_addr = INADDR_ANY;
	}

	void binduj()
	{
		if (bind(s, (sockaddr*)&local, sizeof(local))) // bindowanie
		{
			printf("Blad bindowania na port lokalny\n");
			_getch();
			closesocket(s);
		}
		else { printf("[OK] Bindowanie na port lokalny: %d\n", (int)ntohs(local.sin_port)); }
		printf("_________Dane________\n");
		printf("SERWER IP: %d.%d.%d.%d\n", int(remote.sin_addr.s_addr & 0xFF), int((remote.sin_addr.s_addr & 0xFF00) >> 8),
			int((remote.sin_addr.s_addr & 0xFF0000) >> 16), int((remote.sin_addr.s_addr & 0xFF000000) >> 24));
		printf("SERWER PORT: %d\n", (int)ntohs(remote.sin_port));
		printf("LOCAL IP: %d.%d.%d.%d\n", int(local.sin_addr.s_addr & 0xFF), int((local.sin_addr.s_addr & 0xFF00) >> 8),
			int((local.sin_addr.s_addr & 0xFF0000) >> 16), int((local.sin_addr.s_addr & 0xFF000000) >> 24));
		printf("LOCAL PORT: %d\n", (int)ntohs(local.sin_port));
		printf("_____________________\n");

	}

	void polacz()
	{
		if (connect(s, (sockaddr*)&remote, sizeof(remote)) != 0) // laczenie z remote hostem 0 = sukces
		{
			printf("Blad polaczenia z serwerem zdalnym");
		}
		else { printf("[OK] Polaczono z serwerem\n"); }
	}

	public:

	void wyslij_pakiet_A()
	{
		/* WYSYLANIE */
		printf("\n Podaj wiadomosc do wyslania: ");
		char temp_send[256];
		cout << temp_send;
		//gets_s(temp_send);
		printf("Twoja wiadomosc to: ' %s '\n", temp_send);
		
		while (temp_send[i] != 0) // dlugosc wiadomosci
		{
			i++;
		}
		ile = send(s, temp_send, i, 0); // sizeof(temp_send) dla wyslania calego temp_send[255]
		if (ile <= 0)
		{
			printf("Blad wyslania pakietu!\n");
		}
		else{ printf("Wyslano wiadomosc o dlugosci: %d \n", ile); }
		ile = 0;
	}

	void odbierz()
	{
		/* ODBIERANIE */
		char temp_recv[256];
		ile = recv(s, temp_recv, sizeof(temp_recv), 0);
		if (ile <= 0)
		{
			printf("Blad odbierania pakietu!\n");
		}
		else{
			temp_recv[i] = '\0';
			printf("Odebrano wiadomosc o dlugosci: %d o tresci ' %s '\n", ile, temp_recv);
		}
	}

	void wyslij_pakiet_B() // DANE BINARNE! Tablica intów
	{
		cout<<"\n Podaj ilosc liczb do wyslania: ";
		int iloscl = 0;
		cin >> iloscl;
		cout<<"Liczba liczb to: "<<iloscl<<endl;
		int *liczby = NULL;
		
		if (this != 0)
		{
			liczby = new int[iloscl+2];
		}
		else{
			cout << "error";
			_getch();
		}


		for (int j = 0; j < iloscl; j++)
		{
			cout << "Podaj liczbe nr " << j << ": "; 
			cin >> liczby[j];
		}

		//Skladanie w tablice:
		//Pierwsza komorka = 0x33 - serwer rozpozna jako binarke
		char sendbuf[512];
		int znak = 0x33;
		memcpy(&sendbuf[0], &znak, sizeof(int));
		memcpy(&sendbuf[sizeof(int)], &iloscl, sizeof(int));

		for (int j = 2; j < iloscl+3; j++)
		{
			memcpy(&sendbuf[sizeof(int) * j], &liczby[j - 2], sizeof(int));
		}

		i = 2 + sizeof(int)* iloscl;

		ile = send(s, sendbuf, i, 0); // sizeof(temp_send) dla wyslania calego temp_send[255]
		if (ile <= 0)
		{
			cout<<"Blad wyslania pakietu!"<<endl;
		}
		else{ cout << "Wyslano wiadomosc o dlugosci: " << ile << endl;}


		ile = 0;

	}

	void wyslij_pakiet_C() // DANE BINARNE! Tablica zdañ
	{
		char sendbuf[1024];
		sendbuf[0] = 0x34; //0x34 rozpoznaje ankiete na serwerze
		cout << "Podaj pytanie do ankiety: ";
		string pytanie;
		cin >> pytanie;
		cout << "Ile odpowiedzi ma zawieraæ ankieta? ";
		int ilosc = 0;
		cin >> ilosc;
		int indeks = 1;


		memcpy(&sendbuf[indeks], pytanie.c_str(), sizeof(char)* pytanie.length());
		indeks = indeks + sizeof(char)* pytanie.length();
		sendbuf[indeks] = 0x35; //koniec pytania
		indeks++;
		cout << "indeks = " << indeks;


		for (int j = 0; j < ilosc; j++)
		{
			cout << "Pytanie nr " << j + 1 << ": ";
			string pytanie;
			cin >> pytanie;
			
			memcpy(&sendbuf[indeks], pytanie.c_str(), sizeof(char) * pytanie.length());
			indeks = indeks + sizeof(char) * pytanie.length();
			sendbuf[indeks] = 0x35; //koniec pytania
			indeks++;
			cout << "indeks = " << indeks;
		}
		sendbuf[indeks] = 0x36; //koniec ankiety
		//ZESPOLONO
		_getch();

		char pytanieo[255];
		char znak;

		cout << "Twoja ankieta: " << endl;
		indeks = 0;
		do
		{
			for (int j = indeks; j++; )
			{
				if (sendbuf[j] == 0x35)
				{
					pytanieo[j - 1] = '\n';
					break;
				}

				pytanieo[j - 1] = char(sendbuf[j]);
				znak = sendbuf[j + 1];
			}
			cout << "Pytanie: " << pytanieo;
		}
		while (znak != 0x36);

		
		
		


	}

	void wyslij_pakiet_D()
	{
		string dane;
		dane = "\x44Test";
		send(s, dane.c_str(), dane.length()+1, 0);


	}
};

int main()
{
	WinSock *ws = new WinSock();
	
	int wybor = -1;
	while (wybor != 0)
	{
		cout << "Wybor: ";
		cin >> wybor;

		switch (wybor)
		{
		case 1:
			ws->wyslij_pakiet_A();
			ws->odbierz();
			break;

		case 2:
			ws->wyslij_pakiet_B();
			ws->odbierz();
			break;

		case 3:
			ws->wyslij_pakiet_C();
			ws->odbierz();
			break;

		case 4:
			ws->wyslij_pakiet_D();
			ws->odbierz();
			break;
		
		case 0:
			break;
		default:

			break;
		}
	}
	
	_getch();
	return 0;
}
