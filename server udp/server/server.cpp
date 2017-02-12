// server.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <winsock2.h>
#include <string>
#include <string.h>
#include <conio.h>
#include <fstream>
#include <iostream>

using namespace std;

#pragma comment(lib,"ws2_32.lib") //Winsock Library

#define BUFLEN 512  //Max length of buffer
#define PORT 8888   //The port on which to listen for incoming data



class Rozmowca
{
public:
	string ip_rozmowcy = "PUSTE", ip_adresata = "PUSTE";
	Rozmowca *poprzedni, *kolejny;
	struct sockaddr_in server, si_other;
	int indeks;

	Rozmowca(string _ip_rozmowcy, string _ip_adresata, Rozmowca *_poprzedni, int _indeks)
	{
		ip_rozmowcy = _ip_rozmowcy;
		ip_adresata = _ip_adresata;
		poprzedni = _poprzedni;
		kolejny = NULL;
		indeks = _indeks;
	}

	Rozmowca()
	{


	}

	string get_ip_rozmowcy()
	{
		return ip_rozmowcy;
	}

	string get_ip_adresata()
	{
		return ip_adresata;
	}
	
	void set_kolejny(Rozmowca *_kolejny)
	{
		kolejny = _kolejny;
	}

};

int main()
{
	SOCKET s;
	struct sockaddr_in server, si_other;
	int slen, recv_len;
	char buf[BUFLEN];
	WSADATA wsa;
	

	slen = sizeof(si_other);

	//Initialise winsock
	printf("\nInitialising Winsock...");
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("Failed. Error Code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	printf("Initialised.\n");

	//Create a socket
	if ((s = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
	{
		printf("Could not create socket : %d", WSAGetLastError());
	}
	printf("Socket created.\n");

	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(PORT);

	//Bind
	if (bind(s, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR)
	{
		printf("Bind failed with error code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	puts("Bind done");

	char korektor[] = " ";
	Rozmowca *rozmowca_aktualny = new Rozmowca();
	int liczba_rozmowcow = 0;
	Rozmowca *poprzedni_rozmowca = new Rozmowca();
	Rozmowca *pierwszy_rozmowca = new Rozmowca();
	Rozmowca *ostatni_rozmowca = new Rozmowca();



	//keep listening for data
	while (1)
	{
		printf("Oczekiwanie na dane...");
		fflush(stdout);

		//clear the buffer by filling null, it might have previously received data
		memset(buf, '\0', BUFLEN);

		//try to receive some data, this is a blocking call
		if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) == SOCKET_ERROR)
		{
			printf("recvfrom() failed with error code : %d", WSAGetLastError());
			exit(EXIT_FAILURE);
		}



		//print details of the client/peer and the data received
		printf("Odebrano pakiet od %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
		printf("Dane: %s\n", buf);

		Rozmowca *rozmowca_w_kolejce = ostatni_rozmowca;
		bool przeslij = false;
		string ipodebrane = inet_ntoa(si_other.sin_addr);
				
		while (rozmowca_w_kolejce->poprzedni != NULL)
		{
			cout << "Sprawdzanie nadawcy w bazie danych w celu wyznaczenia docelowego ip odbiorcy."<<endl;
			cout << "IP nadawcy: " << ipodebrane << ", IP sprawdzane: " << rozmowca_w_kolejce->get_ip_rozmowcy() << endl;
			if (ipodebrane.compare(rozmowca_w_kolejce->get_ip_rozmowcy())==0)
			{
				struct sockaddr_in server, si_other2;
				string ip = rozmowca_w_kolejce->get_ip_adresata();
				si_other2.sin_addr.S_un.S_addr = inet_addr(ip.c_str());

				cout << "Wyslano pakiet od nadawcy o IP: " << rozmowca_w_kolejce->get_ip_rozmowcy() << " do odbiorcy o IP: " << ip << "!" << endl;

				sendto(s, buf, strlen(buf), 0, (struct sockaddr *) &si_other2, slen);
				break;
			}
			else
			rozmowca_w_kolejce = rozmowca_w_kolejce->poprzedni;
		}



		//rejstracja uzytkownika
		string s_buf = buf;
		char *next_token1 = NULL;

		if (s_buf.length() > 0)
		{
			s_buf = strtok_s(buf, korektor, &next_token1);

			if (s_buf.compare("zarejestruj") == 0)
			{ 	
				if (liczba_rozmowcow == 0)
				{
					Rozmowca *rozmowca = new Rozmowca(inet_ntoa(si_other.sin_addr), strtok_s(NULL, korektor, &next_token1), poprzedni_rozmowca, liczba_rozmowcow);
					pierwszy_rozmowca = rozmowca;
					rozmowca_aktualny = rozmowca;
					poprzedni_rozmowca = rozmowca;
					cout << "Zarejestrowano pierwszego uzytkownika o ip: " << rozmowca->get_ip_rozmowcy() << ", o adresowaniu adresata: " << rozmowca->get_ip_adresata() << endl;
					ostatni_rozmowca = rozmowca;
					liczba_rozmowcow++;
					ostatni_rozmowca = rozmowca;
				}
				else if (liczba_rozmowcow>0)
				{
					Rozmowca *rozmowca = new Rozmowca(inet_ntoa(si_other.sin_addr), strtok_s(NULL, korektor, &next_token1), poprzedni_rozmowca, liczba_rozmowcow);
					rozmowca_aktualny->set_kolejny(rozmowca);
					poprzedni_rozmowca = rozmowca;
					cout << "Zarejestrowano uzytkownika o ip: " << rozmowca->get_ip_rozmowcy() << ", o adresowaniu adresata: " << rozmowca->get_ip_adresata() << endl;
					liczba_rozmowcow++;
					ostatni_rozmowca = rozmowca;
				}
			}
			if (buf[0] == ']')
			{
				char dlbbuf[] = {buf[1], buf[2]};

				unsigned double liczba = 0;
				memcpy(&liczba, &dlbbuf, 2);


				cout << "Odebrana liczba binarna: " << liczba <<endl;
			}


			//now reply the client with the same data
			if (sendto(s, buf, recv_len, 0, (struct sockaddr*) &si_other, slen) == SOCKET_ERROR)
			{
				printf("sendto() failed with error code : %d", WSAGetLastError());
				exit(EXIT_FAILURE);
			}
		}
		else {
			cout << "Wystapilo zbyt krotkie polecenie!<<" << endl;
			sendto(s, buf, recv_len, 0, (struct sockaddr*) &si_other, slen);
			}
	}

	closesocket(s);
	WSACleanup();

	return 0;
}