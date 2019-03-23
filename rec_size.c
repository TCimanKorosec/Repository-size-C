/*
 OCEANE ZHANG
 THOMAS CIMAN
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>


void* taille(void* repository){
    int taille_Arg = strlen((char*) repository);
    char* repo = (char*) malloc((taille_Arg+1) * sizeof(char));
    strcpy(repo, repository);
    DIR* directory = opendir(repo);
    pthread_t th;

    if(directory == NULL){
        printf("Erreur la chaine de caractère passée en paramètre n'existe pas dans l'arborescence du systeme\n");
        
        free(repo);
        
        exit(1);
    }

	struct dirent* dir;
	struct stat* status = malloc(sizeof(struct stat));

    void* retour;
    void* size = 0;


	while((dir = readdir(directory)) != NULL){

        if(dir->d_name[0] != '.'){

            char* new_rep = (char*) malloc((strlen(repo) + strlen(dir->d_name) + 2)*sizeof(char));
            strcpy(new_rep, repo);

            if(repo[strlen(repo) - 1] != '/'){
                    strcat(new_rep, "/");
                }

            strcat(new_rep, dir->d_name);

            // SI LE FICHIER EST UN REPERTOIRE
            if(dir->d_type == 4){

                if((pthread_create(&th, NULL, taille, new_rep)) < 0){
                    fprintf(stderr, "Erreur lors de la création du thread\n");

                    exit(1);
                }
                pthread_join(th, &retour);
                size += (unsigned long) retour;
            }

            (stat(new_rep, status));

            size += status->st_size;

            //printf("%s \t\t\t %lu\n", new_rep, (unsigned long) size);

            free(new_rep);
		}
	}

	//FERMETURE REPERTOIRE
    closedir(directory);
    //ON LIBERE LA MEMOIRE
    free(status);
    free(repo);

    pthread_exit(size);
}



int main(int argc, char* argv[]){
	if(argc != 2){
		fprintf(stderr ,"Erreur 1 argument attendu\n");

		return EXIT_FAILURE;
	}


    pthread_t thread1;

	char* argument = (char*)malloc((strlen(argv[1]) + 1)*sizeof(char));
	strcat(argument, argv[1]);
	struct stat* status = malloc(sizeof(struct stat));

	stat(argument, status);
	unsigned long size = status->st_size;


    //La taille en retour
    void* retour;

    DIR* rep;
	unsigned long sommeTotale = size;

	if((rep = opendir(argument)) == NULL){
        printf("\nCeci n'est pas un répertoire\n\n");
        printf("\nL'argument entré a une taille de %lu\n", sommeTotale);
	}
	else{
		closedir(rep);
		printf("\nVous avez bien entré un répertoire\n\n");

        printf("%s \t\t\t%lu\n", argument, (unsigned long)status->st_size);

        if(pthread_create(&thread1, NULL, taille, argument) < 0){
            fprintf(stderr, "Erreur lors de la création du thread\n");

            exit(1);
        }
        pthread_join(thread1, &retour);
    sommeTotale += (unsigned long) retour;

	}


	printf("La somme totale est %lu\n\n", sommeTotale);

    //LIBERE LA MEMOIRE
    free(status);
	free(argument);
	return EXIT_SUCCESS;
}
