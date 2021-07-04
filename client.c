#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"
#include "parson.h"
#define LEN 100
#define BUFLENG 20

void register_command(int sockfd){

    char username[BUFLENG];
    char password[BUFLENG];

    char *message;
    char *payload;
    char *response;

    printf("username=");
    scanf("%s", username);
 
    printf("password=");
    scanf("%s", password);

    sockfd = open_connection("34.118.48.238", 8080, AF_INET, SOCK_STREAM, 0);

    payload = create_register_payload(username, password);

    message = compute_post_request("34.118.48.238", "/api/v1/tema/auth/register", "application/json", &payload, 1, NULL, 0, NULL, 0);

    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);


    if(strstr(response, "error") != NULL){
        printf("Username is already taken!\n");
    } else if(strstr(response, "error") == NULL){
        printf("Success!\n");
    }

    free(message);
    free(response);
    free(payload);

}

char* login_command(int sockfd, char *cookie[1]){

	if(strcmp(cookie[0], "empty_cookie") != 0){

		printf("Someone else is logged in!\n");
		return cookie[0];
	} else {

    char username[BUFLENG];
    char password[BUFLENG];

    char *message;
    char *payload;
    char *response;

    printf("username=");
    scanf("%s", username);

    printf("password=");
    scanf("%s", password);

    sockfd = open_connection("34.118.48.238", 8080, AF_INET, SOCK_STREAM, 0);

    payload = create_register_payload(username, password);


    message = compute_post_request("34.118.48.238", "/api/v1/tema/auth/login", "application/json", &payload, 1, NULL, 0, NULL, 0);

    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);

    if(strstr(response, "error") != NULL){

        printf("Credentials are not good!\n");

    } else if(strstr(response, "error") == NULL){
        
    	char *find = strstr(response, "connect.sid=");
    	find = strtok(find, ";");
       	return find;

    }

    free(message);
    free(response);
    free(payload);

    return NULL;
}

}

char* enter_library(int sockfd, char* cookie[1]){

	if(strcmp(cookie[0], "empty_cookie") == 0){

		printf("You are not logged in!\n");
		return NULL;
	}

    char *message;
    char *response;

    sockfd = open_connection("34.118.48.238", 8080, AF_INET, SOCK_STREAM, 0);

    message = compute_get_request("34.118.48.238", "/api/v1/tema/library/access", NULL, cookie, 1, NULL, 0);

    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);

    if(strstr(response, "error") != NULL){

        printf("Error occured!\n");

    } else {
        
    	char *find = strstr(response, "token\":\"");
    	find = find + 8;
    	find = strtok(find, "\"");
	   	return find;
	   }

    free(message);
    free(response);

    return NULL;
}

void get_books(int sockfd, char* cookie[1], char* token[1]){

	sockfd = open_connection("34.118.48.238", 8080, AF_INET, SOCK_STREAM, 0);

	if(strcmp(cookie[0], "empty_cookie") == 0){
		printf("You are not logged in!\n");
	} else if(strcmp(token[0], "empty_token") == 0){
		printf("You are not allowed to this library!\n");
	} else {

    char *message;
    char *response;

    message = compute_get_request("34.118.48.238", "/api/v1/tema/library/books", NULL, cookie, 1, token, 1);

    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);

    if(strstr(response, "error") != NULL){

        printf("Error occured!\n");

    } else {
    	if(basic_extract_json_response(response) != NULL){
    		printf("%s\n", basic_extract_json_response(response));
    	} else {
    		printf("Nu exista carti momentan!\n");
    	}
    }

    free(message);
    free(response);
	}
}

void get_book(int sockfd, char* cookie[1], char* token[1]){

	sockfd = open_connection("34.118.48.238", 8080, AF_INET, SOCK_STREAM, 0);

	if(strcmp(cookie[0], "empty_cookie") == 0){
		printf("You are not logged in!\n");
	} else if(strcmp(token[0], "empty_token") == 0){
		printf("You are not allowed to this library\n");
	} else {

	char *id = (char *)malloc(LEN * sizeof(char)) ;
    char *message;
    char *response;

    printf("id=");
    scanf("%s", id);

    char *path = (char *)malloc(LEN * sizeof(char));
    strcpy(path, "/api/v1/tema/library/books/");
    strcat(path, id);

    message = compute_get_request("34.118.48.238", path, NULL, cookie, 1, token, 1);

    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);

    if(strstr(response, "error") != NULL){

        printf("Cartea cu id-ul respectiv nu exista!\n");

    } else {
    	if(basic_extract_json_response(response) != NULL){
    		printf("%s\n", basic_extract_json_response(response));
    	} else {
    		printf("Nu exista carti!\n");
    	}
    }

    free(message);
    free(response);
	}
}

void add_book(int sockfd, char *cookie[1], char* token[1]){

	 sockfd = open_connection("34.118.48.238", 8080, AF_INET, SOCK_STREAM, 0);

	if(strcmp(cookie[0], "empty_cookie") == 0){
		printf("You are not logged in!\n");
	} else if(strcmp(token[0], "empty_token") == 0){
		printf("You are not allowed to this library\n");
	} else {


    char title[BUFLENG];
    char author[BUFLENG];
    char genre[BUFLENG];
    char publisher[BUFLENG];
    char page_count[BUFLENG];

    char *message;
    char *payload;
    char *response;

    printf("title=");
    scanf("%s", title);
    printf("author=");
    scanf("%s", author);
    printf("genre=");
    scanf("%s", genre);
    printf("publisher=");
    scanf("%s", publisher);
    printf("page_count=");
    scanf("%s", page_count);

    payload = create_book_payload(title, author, genre, publisher, page_count);

    message = compute_post_request("34.118.48.238", "/api/v1/tema/library/books" , "application/json", &payload, 1, NULL, 0, token, 1);

    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);

    if(strstr(response, "error") != NULL){
        printf("Could not add book!\n");
    } else if(strstr(response, "error") == NULL){
    	printf("Book was added successfully!\n");
    }

    free(message);
    free(response);
    free(payload);
}

}


 void delete_book(int sockfd, char *cookie[1], char* token[1]){


	if(strcmp(cookie[0], "empty_cookie") == 0){
		printf("You are not logged in!\n");
	} else if(strcmp(token[0], "empty_token") == 0){
		printf("You are not allowed to this library\n");
	} else {

    char id[LEN];

    char *message;
    char *response;

    printf("id=");
    scanf("%s", id);

    sockfd = open_connection("34.118.48.238", 8080, AF_INET, SOCK_STREAM, 0);

    char *path = (char *)malloc(LEN * sizeof(char));
    strcpy(path, "/api/v1/tema/library/books/");
    strcat(path, id);

    message = compute_delete_request("34.118.48.238", path, cookie, 1, token, 1);

    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);

    if(strstr(response, "error") != NULL){
        printf("Could not find book with given id!\n");
    } else if(strstr(response, "error") == NULL){
        printf("Book deleted successfully!\n");
    }

    free(message);
    free(response);
	}

}

void logout(int sockfd, char *cookie[1], char *token[1]){

	if(strcmp(cookie[0], "empty_cookie") == 0){
		printf("You are not logged in!\n");
	} else {

    char *message;
    char *response;

    sockfd = open_connection("34.118.48.238", 8080, AF_INET, SOCK_STREAM, 0);

    message = compute_get_request("34.118.48.238", "/api/v1/tema/auth/logout", NULL, cookie, 1, NULL, 0);

    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);

    if(strstr(response, "error") != NULL){
        printf("Error occured during logout!\n");
    } else if(strstr(response, "error") == NULL){
        printf("You successfully logged out!\n");
    }

    strcpy(cookie[0], "empty_cookie");
    strcpy(token[0], "empty_token");

    free(message);
    free(response);
}	

}

int main(int argc, char *argv[])
{   
    int sockfd;
    char command[15];

    char *cookie[1];
    cookie[0] = (char *)malloc(LEN * sizeof(char));
    strcpy(cookie[0], "empty_cookie");

    char *token[1];
    token[0] = (char *)malloc(LEN * sizeof(char));
    strcpy(token[0], "empty_token");

    sockfd = open_connection("34.118.48.238", 8080, AF_INET, SOCK_STREAM, 0);

    while(1){

        scanf("%s", command);

        if(strncmp(command, "exit", 4) == 0){

            close_connection(sockfd);
            break;

        } else if(strncmp(command, "register", 8) == 0){
           
            register_command(sockfd);
           
           } else if(strncmp(command, "login", 5) == 0){

           	cookie[0] = login_command(sockfd, cookie);

           } else if(strncmp(command, "enter_library", 13) == 0){

           		token[0] = enter_library(sockfd, cookie);
           } else if(strncmp(command, "get_books", 9) == 0){

           		get_books(sockfd, cookie, token);

           } else if(strncmp(command, "get_book", 8) == 0){

           		get_book(sockfd, cookie, token);
           } else if(strncmp(command, "add_book", 8) == 0){

           		add_book(sockfd, cookie, token);
           } else if(strncmp(command, "delete_book", 11) == 0){

           	  	delete_book(sockfd, cookie, token);
           } else if(strncmp(command, "logout", 6) == 0){

           		logout(sockfd, cookie, token);
           } else {

           		printf("This command does not exist!\n");
           }

         }
 
    return 0;
}
