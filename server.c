#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h> 
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <dirent.h>
#include <errno.h>
#include <resolv.h>
#include <netinet/in.h>
#include <sys/wait.h>

char buf[100000];   //buffer
char buff[512];	    //mniejszy buffer
char login[10];		
char haslo[10];
char zatwierdzenie[3];	
char path[512];		//sciezka do katalogu ostatniego obslugiwanego uzytkownika 
char czylogged[2];	// czy uzytkownik jest aktualnie zalogowany
int porty[5][2];	//tworzymy 5 dodatkowych portow do obslugi wielu uzytkownikow (porty[i][j] , i=jesli 0 to znaczy ze port wolny , w przeciwnym wypadku znajduje sie tu pid procesu ktory go wykorzystuje , j = port)
bool wolnyport = true;  //zmienna informujaca czy jakikolwiek port jest wolny
int liczbaPortow = 5;	//liczba zdeklarowanych portow do wykorzystania

int main(int argc, char **argv)
{
	////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////
	
	for(int i=0 ;i<liczbaPortow ; i++){
		for (int j=0 ; j<liczbaPortow ; j++) {		//inicjujemy tablice z portami
	porty[i][j] = 0;
		}
	}
	
    porty[0][1] = 4445;
    porty[1][1] = 4446;
    porty[2][1] = 4447;		//przypisujemy porty do tablicy
    porty[3][1] = 4448;
    porty[4][1] = 4449;
    	
    struct sockaddr_in myaddr, endpoint;
    int sdsocket, r;
    socklen_t addrlen;								//tworzenie i inicjacja zmiennych
    unsigned int port;
    port = 4444;
    int pid;
    char nazwaPliku[15];	
	DIR *mydir;
	struct dirent *myfile;
	struct stat mystat;
	//////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////
	

    if ((sdsocket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        printf("socket() nie powiodl sie\n");
        return 1;
    }
        
    myaddr.sin_family = AF_INET;
    myaddr.sin_port = htons(port);
    myaddr.sin_addr.s_addr = INADDR_ANY;
														//tworzenie i bindowanie socketu
    if (
        bind(sdsocket,
             (struct sockaddr*) &myaddr,
             sizeof(struct sockaddr_in)) < 0) {
        printf("bind() nie powiodl sie\n");
        return 1;
    }
		
    addrlen = sizeof(struct sockaddr_in);

	
	////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
////////////////////////////////////////////////////
	
	
	
	
	
	
	
	
	
	
	
	
	while (1){  //glowna petla programu
	
	
	///////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////	
	while(1) //w tej petli pobieramy informacje czy ktos jest zalogowany (wszystkie inne odrzuca)
	{	
	r = recvfrom(sdsocket,
                     czylogged,
                     1,
                     0,
                     (struct sockaddr*) &endpoint,       //pobranie danych od clienta o zalogowaniu
                     &addrlen);
                     
                     
	int status;	
	wolnyport = false;
	
	
	for (int i=0 ; i<liczbaPortow ; i++) {
		waitpid(porty[i][0],&status,WNOHANG);
		if (WIFEXITED(status)) {					//zwalniamy porty od procesow ktore sie juz zakonczyly 
			if (WEXITSTATUS(status) == 2)
				porty[i][0] = 0;
		}
		
	if (porty[i][0] == 0) wolnyport = true;		//sprawdzamy czy jakis port jest wolny 
	}
	
	
	if (czylogged[0] == 57 || czylogged[0] == 48) break;	//sprawdzamy czy uzytkownik wyslal informacje na temat swojego logowania , jesli tak to wyjdz z petli i go obsluz
	}
	
	//////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////
	



	///////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////
	if (czylogged[0] == 48){    //jesli uzytkownik jest niezalogowany

		r = recvfrom(sdsocket,
                     zatwierdzenie,
                     1,								
                     0,										//pobieramy informacje czy chce sie zarejestrwac czy zalogowac. 
                     (struct sockaddr*) &endpoint,
                     &addrlen);
	
		memset(login, 0, 10);
		memset(haslo,0,10);

		switch(zatwierdzenie[0]) { 

			case '1' : //obsluga zalogowania
	
				r = recvfrom(sdsocket,
                     login,
                     10,
                     0,
                     (struct sockaddr*) &endpoint,
                     &addrlen);
				r = recvfrom(sdsocket,
                     haslo,			//pobieramy login i haslo
                     10,
                     0,
                     (struct sockaddr*) &endpoint,
                     &addrlen);
		
		
					login[strlen(login)] = '\0';
					haslo[strlen(haslo)] = '\0';
					chdir("/home/adrian/Pulpit/Katalog/");
	
					if (access(login,F_OK) != -1) {
						chdir(("/home/adrian/Pulpit/Katalog/%s",login));
						if (access(haslo,F_OK) != -1) {
								sprintf(path, "/home/adrian/Pulpit/Katalog/%s",login);		//sprawdzamy dane logowania
			
								r = sendto(sdsocket,
            					   "zalogowano pomyslnie",
								   21,
            					   0,
            	 				  (struct sockaddr*) &endpoint,
            	 				   sizeof(endpoint));
			
								mydir = opendir(path);
								
								while ((myfile = readdir(mydir)) != NULL)		//wypisujemy wszystkie pliki na koncie uzytkownika 
								{
									sprintf(buff,"%s/%s",path,myfile->d_name);
									stat(buff, &mystat);
		
		
									if (myfile->d_name[0] == '.') {}
									
									else if (strlen(myfile->d_name) == strlen(haslo)) {
										for (int i=0 ; i<strlen(haslo) ;i++) {
											if (myfile->d_name[i] == haslo[i]) {}
											
											else {
												r = sendto(sdsocket,
            	 								  ("%s",myfile->d_name),
            									   40,
            									   0,
            									   (struct sockaddr*) &endpoint,
            	 						 		  sizeof(endpoint));
											break;
											}
										}
									}
									else{
										r = sendto(sdsocket,
            						   ("%s",myfile->d_name),
            						   40,
            						   0,
            						   (struct sockaddr*) &endpoint,
            						    sizeof(endpoint));
									}
								}
	
								r = sendto(sdsocket,
         					   	   "q",
           					 	   1,
            					   0,
            					   (struct sockaddr*) &endpoint,
            					    sizeof(endpoint));
									closedir(mydir);
						}
						
						
						else {
							r = sendto(sdsocket,
        			    	   "zle haslo",
          				  	   10,								
           				 	   0,
            				   (struct sockaddr*) &endpoint,
            				    sizeof(endpoint));
						}
					}

					else 
					{
						r = sendto(sdsocket,
            			   "nie ma takiego loginu",
            			   22,
            			   0,
            			   (struct sockaddr*) &endpoint,
            			    sizeof(endpoint));
					}
			break;
	
			case '2' :  //gdy uzytkownik chce sie zarejestrowac

				r = recvfrom(sdsocket,
                     login,
                     10,							
                     0,
                     (struct sockaddr*) &endpoint,
                     &addrlen);
				r = recvfrom(sdsocket,
                     haslo,
                     10,
                     0,
                     (struct sockaddr*) &endpoint,		//pobieramy dane do rejestracji
                     &addrlen);
                     
				pid = fork();
				if (pid>0) break;
				
				login[strlen(login)] = '\0';
				haslo[strlen(haslo)] = '\0';
				chdir("/home/adrian/Pulpit/Katalog/");
				
      			if (access(login,F_OK) != -1) {
				r = sendto(sdsocket,
            	   "twoj login juz istnieje!",
            	   25,
            	   0,
            	   (struct sockaddr*) &endpoint,
            	    sizeof(endpoint));		
				}
				else 
				{
					int folder = mkdir(("/home/adrian/Pulpit/Katalog/%s",login),0777);
					chdir(("/home/adrian/Pulpit/Katalog/%s",login));
					int nowehaslo = open(("/home/adrian/Pulpit/Katalog/%s",haslo),O_RDWR|O_CREAT,0777);
					
					r = sendto(sdsocket,
            	 	  "Zarejestrowano pomyslnie",
            		   25,
            		   0,
            	 	  (struct sockaddr*) &endpoint,
            		    sizeof(endpoint));
				}
		close(sdsocket);

		return 2;
		break;
		}
	}
	/////////////////////////////////////////////////////////////	
	////////////////////////////////////////////////////////////
	
	
	
	
	/////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////
	if (czylogged[0] == 57) {
		
		memset(login, 0, 10);
		memset(haslo,0,10);
		
		r = recvfrom(sdsocket,
                     login,
                     10,
                     0,
                     (struct sockaddr*) &endpoint,
                     &addrlen);
		r = recvfrom(sdsocket,
                     haslo,
                     10,
                     0,
                     (struct sockaddr*) &endpoint,
                     &addrlen);
		login[strlen(login)] = '\0';
		haslo[strlen(haslo)] = '\0';

		r = recvfrom(sdsocket,
                     zatwierdzenie,
                     10,
                     0,
                     (struct sockaddr*) &endpoint,
                     &addrlen);
                     
                     
		switch(zatwierdzenie[0]) {
	
			case '2' :  //odswiezanie zawartosci katalogu
			memset(path,0,512);
			
			r = recvfrom(sdsocket,
                     path,
                     512,
                     0,
                     (struct sockaddr*) &endpoint,
                     &addrlen);
                     
			pid = fork();
			if (pid >0) break;
			
			path[strlen(path)] = '\0';
			mydir = opendir(path);
			
			while ((myfile = readdir(mydir)) != NULL)
			{
		
				sprintf(buff,"%s/%s",path,myfile->d_name);
				stat(buff, &mystat);
		
				if (myfile->d_name[0] == '.') {}
				
				else if (strlen(myfile->d_name) == strlen(haslo)) {
					for (int i=0 ; i<strlen(haslo) ;i++) {
						if (myfile->d_name[i] == haslo[i]) {}
						else {
							r = sendto(sdsocket,
            				   ("%s",myfile->d_name),
            				   40,
            				   0,
            	 			  (struct sockaddr*) &endpoint,
            	  			  sizeof(endpoint));
								break;
							}
					}
				}
				
				else{
					r = sendto(sdsocket,
            		   ("%s",myfile->d_name),
            		   40,
            		   0,
            		   (struct sockaddr*) &endpoint,
            	 	   sizeof(endpoint));
				}
			}
	
			r = sendto(sdsocket,
            	   "q",
            	   1,
            	   0,
            	   (struct sockaddr*) &endpoint,
            	    sizeof(endpoint));
            	    
			closedir(mydir);
			close(sdsocket);
		
			return 2;
			break;



	case '3' : //wrzucanie pliku na server
	
		r = recvfrom(sdsocket,
                     path,
                     512,
                     0,
                     (struct sockaddr*) &endpoint,
                     &addrlen);
		r = recvfrom(sdsocket,
                     buf,
                     512,
                     0,
                     (struct sockaddr*) &endpoint,
                     &addrlen);
                     
		if (wolnyport == true){
			pid = fork();
			int i = 0;
			for (i; i<liczbaPortow ; i++) {
				if (porty[i][0] == 0) {
				porty[i][0] = pid;
				break;
				}
			}
			
			if (pid > 0) break;
			char pomoc[4];
			sprintf(pomoc,"%d",porty[i][1]);
			
			

			//close(sdsocket);

			struct sockaddr_in myaddr2, endpoint2;
    		int sdsocket2;
   			socklen_t addrlen2;
   			unsigned int port2;
    		port2 = porty[i][1];
	

   			if ((sdsocket2 = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
      		    printf("socket() nie powiodl sie\n");
       			return 1;
   			}
        
  			myaddr2.sin_family = myaddr.sin_family;
   			myaddr2.sin_port = htons(port2);
    		myaddr2.sin_addr.s_addr = myaddr.sin_addr.s_addr;

    		if (bind(sdsocket2,(struct sockaddr*) &myaddr2,sizeof(struct sockaddr_in)) < 0) {
        		printf("bind() nie powiodl sie\n");
        		return 1;
    		}
		
    		addrlen2 = sizeof(struct sockaddr_in);

	
			int file = open(("%s/%s",path,buf),O_RDWR|O_CREAT,0777);
			close(file);
			FILE *fp = fopen(("%s/%s",path,buf),"w");
			int ile = 0;
			memset(buf,0,512);

			r = sendto(sdsocket,
            	   pomoc,
            	   4,
            	   0,
            	   (struct sockaddr*) &endpoint,
            	    sizeof(endpoint));

			while(1) {
				memset(buf,0,ile);
				r = recvfrom(sdsocket2,
                     buf,
                     10,
                     0,
                     (struct sockaddr*) &endpoint2,
                     &addrlen2);

				ile = atoi(buf);
				if (ile <10) memset(buf,0,10);

				r = recvfrom(sdsocket2,
                     buf,
                     ile,
                     0,
                     (struct sockaddr*) &endpoint2,
                     &addrlen2);
				
				if (buf[0] == 'q') break;

				fprintf(fp,"%s\n",buf);
			}
	
			fclose(fp);
			fp = NULL;
			
			memset(buf,0,512);
			mydir = opendir(path);
			
			while ((myfile = readdir(mydir)) != NULL)
			{
				sprintf(buff,"%s/%s",path,myfile->d_name);
				stat(buff, &mystat);
				
				if (myfile->d_name[0] == '.') {}
				else if (strlen(myfile->d_name) == strlen(haslo)) {
					for (int i=0 ; i<strlen(haslo) ;i++) {
						if (myfile->d_name[i] == haslo[i]) {}
						else {
							r = sendto(sdsocket2,
            	 			  ("%s",myfile->d_name),
            	 			  40,
            	 			  0,
            	  			 (struct sockaddr*) &endpoint2,
            	   			 sizeof(endpoint));
							break;
						}
					}
				}
				
				else{
					r = sendto(sdsocket2,
            	 	  ("%s",myfile->d_name),
            	 	  40,
            	 	  0,
            	 	  (struct sockaddr*) &endpoint2,
            	 	   sizeof(endpoint2));
				}
				
			}
			
			
			r = sendto(sdsocket2,
            	   "q",
            	   1,
            	   0,
            	   (struct sockaddr*) &endpoint2,
            	    sizeof(endpoint2));
            	    
			closedir(mydir);	
			//close(sdsocket2);
			//close(sdsocket);
			return 2;

		}
		else {
			r = sendto(sdsocket,
            	   ".",
            	   1,
            	   0,
            	   (struct sockaddr*) &endpoint,
            	    sizeof(endpoint));
		}
		break;


	case '4' :	//pobieranie pliku z servera

		memset(path, 0, 512);

		r = recvfrom(sdsocket,
                     path,
                     256,
                     0,
                     (struct sockaddr*) &endpoint,
                     &addrlen);
                     
		if (wolnyport == true){
			
			pid = fork();
			int i = 0;
		
			for (i; i<liczbaPortow ; i++) {
				if (porty[i][0] == 0) {
					porty[i][0] = pid;
					break;
				}
			}
		if (pid > 0) break;
		
			char pomoc[4];
			sprintf(pomoc,"%d",porty[i][1]);
			r = sendto(sdsocket,
            	   pomoc,
            	   4,
            	   0,
            	   (struct sockaddr*) &endpoint,
            	    sizeof(endpoint));

			close(sdsocket);

			struct sockaddr_in myaddr2, endpoint2;
   			int sdsocket2;
    		socklen_t addrlen2;
    		unsigned int port2;
    		port2 = porty[i][1];
	

    		if ((sdsocket2 = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        		printf("socket() nie powiodl sie\n");
       			return 1;
    		}
        
    		myaddr2.sin_family = myaddr.sin_family;
    		myaddr2.sin_port = htons(port2);
    		myaddr2.sin_addr.s_addr = myaddr.sin_addr.s_addr;

    		if (
       		 bind(sdsocket2,
             (struct sockaddr*) &myaddr2,
             sizeof(struct sockaddr_in)) < 0) {
      		  printf("bind() nie powiodl sie\n");
      		  return 1;
   			 }
		
    		addrlen2 = sizeof(struct sockaddr_in);
    		
    		
			r = recvfrom(sdsocket2,
                     buf,
                     10,
                     0,
                     (struct sockaddr*) &endpoint2,
                     &addrlen2);
                     
			if (access(path,F_OK) != -1) {
				FILE *fp = fopen(path,"r");
				while(1) {
					memset(buf, 0, 512);
					if (fgets(buf,512,fp) == NULL) break;
					
					r = sendto(sdsocket2,
            		   buf,
            		   strlen(buf),
            		   0,
            	 	  (struct sockaddr*) &endpoint2,
            	 	   sizeof(endpoint2));
				}
		
				r = sendto(sdsocket2,
            	   ".",
            	   1,
            	   0,
            	   (struct sockaddr*) &endpoint2,
            	    sizeof(endpoint2));
			}
			else {
		
				r = sendto(sdsocket2,
            	   ".",
            	   1,
            	   0,
            	   (struct sockaddr*) &endpoint2,
            	    sizeof(endpoint2));
			}
	

			close(sdsocket2);	
		}
		
		else {
			r = sendto(sdsocket,
            	   ".",
            	   1,
            	   0,
            	   (struct sockaddr*) &endpoint,
            	    sizeof(endpoint));
		}
		
		close(sdsocket);
		return 2;
		break;

	case '5' :  //usuwanie pliku z servera
	
		memset(path, 0, 512);
		r = recvfrom(sdsocket,
                     path,
                     256,
                     0,
                     (struct sockaddr*) &endpoint,
                     &addrlen);
		pid = fork();
		if (pid > 0 ) break;;
		
		if (access(path,F_OK) != -1) {
			int delete = remove(path);
					
			if (delete ==0){
				r = sendto(sdsocket,
            	   "plik zostal usuniety",
            	   30,
            	   0,
            	   (struct sockaddr*) &endpoint,
            	    sizeof(endpoint));
			}

			else {
				r = sendto(sdsocket,
            	   "plik nie zostal usuniety",
            	   30,
            	   0,
            	   (struct sockaddr*) &endpoint,
            	    sizeof(endpoint));
			}

		}
		
		else {
			r = sendto(sdsocket,
            	   "nie istnieje",
            	   20,
            	   0,
            	   (struct sockaddr*) &endpoint,
            	    sizeof(endpoint));
		}
	
		close(sdsocket);
		return 2;

	
	break;

	}//switch
	}//zalogowany
	}//glowna petla

    return 0;
}
