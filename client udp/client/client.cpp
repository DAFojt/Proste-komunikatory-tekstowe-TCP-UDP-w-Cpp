// client.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <winsock2.h>
#include <string>
#include <string.h>
#include <conio.h>
#include <fstream>
#include <iostream>

#pragma comment(lib,"ws2_32.lib") //Winsock Library

//#define SERVER "77.254.29.93"  //ip address of udp server
#define SERVER "192.168.1.123"  //ip address of udp server
#define BUFLEN 512  //Max length of buffer
#define PORT 8888   //The port on which to listen for incoming data

using namespace std;

string wczytaj(std::string nazwaPliku)
{
	ifstream plik;
	plik.open(nazwaPliku);



	char bufor[1024];
	string slowo = "";
	string zdanie = "";

	while (!plik.eof())
	{
		getline(plik, slowo);
		zdanie = zdanie + slowo + " ";
	}

	return zdanie;
}

int main(void)
{
	struct sockaddr_in si_other;
	int s, slen = sizeof(si_other);
	char buf[BUFLEN] = "START";
	char message[BUFLEN] = "START";
	WSADATA wsa;

	//Initialise winsock
	printf("\nInicjalizacja socketu...");
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("Failed. Error Code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	printf("Initialised.\n");

	//create socket
	if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR)
	{
		printf("socket() failed with error code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}

	//setup address structure
	memset((char *)&si_other, 0, sizeof(si_other));
	si_other.sin_family = AF_INET;
	si_other.sin_port = htons(PORT);
	si_other.sin_addr.S_un.S_addr = inet_addr(SERVER);

	//communicatnion choice
	cout << "Witamy w komunikatorze!" << endl << "Za pomoca programu mozesz wysylac wiadomosci do danego uzytkownika." << endl << "Aby utworzyc konto wpisz 'zarejestruj [adres ip adresata]', klamry i cudzyslowia pomijamy ;)";
	int wybor = 0;
	while ((wybor != 1) && (wybor != 2) && (wybor != 3))
	{
		cout << endl << "1: Klawiatura, 2: Plik, 3: Liczba double binarnie. Wybor: ";
		cin >> wybor;
	}
	if (wybor == 1)
		cout << endl << "Wpisanie [exit] konczy program." << endl;

	bool ifexit = false;
	//start communication
	char *next_token1 = NULL;
	while (1)
	{
		switch (wybor)
		{
		case 1:
		{
				  printf("> ");
				  gets_s(message);

				  char message_b[BUFLEN];
				  strcpy_s(message_b, message);
				  char korektor[] = " ";
				  string porownanie = message;
				  string pierwszeslowo = strtok_s(message_b, korektor, &next_token1);

				  if (porownanie.compare("exit") == 0)
					  ifexit = true;

				  if (pierwszeslowo.compare("zarejestruj") == 0)
					  cout << "Stworzono uzytkownika!"<<endl;

				  break;
		}
		case 2:
		{
				  strcpy_s(message, wczytaj("dane.txt").c_str());
				  break;
		}
		case 3:
		{
				  unsigned double liczba = 0;
				  cout << "Liczba do wyslania binarnie: ";
				  cin >> liczba;

				  strcpy_s(message, "123");
				  char buf_message_double[3];
				  message[0] = ']';

				  memcpy(&message[1], &liczba, 2);

				  cout << "Wysylanie danych binarnych: Liczba binarna: " << liczba << ", kod binarny liczby: B1 - " << message[1] << " B2 - " << message[2] << endl;
		}
		}

		if (ifexit == true)
			break;

		//send the message
		if (sendto(s, message, strlen(message), 0, (struct sockaddr *) &si_other, slen) == SOCKET_ERROR)
		{
			printf("sendto() failed with error code : %d", WSAGetLastError());
			exit(EXIT_FAILURE);
		}

		//receive a reply and print it
		//clear the buffer by filling null, it might have previously received data
		memset(buf, '\0', BUFLEN);
		//try to receive some data, this is a blocking call
		if (recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen) == SOCKET_ERROR)
		{
			printf("recvfrom() failed with error code : %d", WSAGetLastError());
			exit(EXIT_FAILURE);
		}
		if(wybor == 2)
			break;

		//cout << "Odebrano wiadomosc! Tresc: " << buf;

	}

	cout << "Zakonczono program. Nastepuje czyszczenie pamieci. Kliknij ENTER.";
	_getch();
	closesocket(s);
	WSACleanup();
	return 0;
}