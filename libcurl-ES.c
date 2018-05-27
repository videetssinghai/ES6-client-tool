#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <strings.h>
#include <stdlib.h>

struct string {
  char *ptr;
  size_t len;
};

void init_string(struct string *s) {
  s->len = 0;
  s->ptr = malloc(s->len+1);
  if (s->ptr == NULL) {
    fprintf(stderr, "malloc() failed\n");
    exit(EXIT_FAILURE);
  }
  s->ptr[0] = '\0';
}

size_t writefunc(void *ptr, size_t size, size_t nmemb, struct string *s) {

  size_t new_len = s->len + size*nmemb;
  s->ptr = realloc(s->ptr, new_len+1);
  if (s->ptr == NULL) {
    fprintf(stderr, "realloc() failed\n");
    exit(EXIT_FAILURE);
  }
  memcpy(s->ptr+s->len, ptr, size*nmemb);
  s->ptr[new_len] = '\0';
  s->len = new_len;

  return size*nmemb;
}

CURL *curl;
CURLcode res;
char *value = NULL;
struct string s;
struct curl_slist *headers = NULL;

void init() {

  init_string(&s);
  curl_global_init(CURL_GLOBAL_DEFAULT);
  curl = curl_easy_init();
  printf("%s\n", "init called");

}

void deinit() {

  free(s.ptr);
  curl_global_cleanup();
  printf("%s\n", "deinit called");
}

char *build_url(char* server, char* query) {

  char *request ="";
  if((request = malloc(strlen(query)+strlen(server)+1)) != NULL){
    request[0] = '\0';   // ensures the memory is an empty string
    strcat(request,server);
    strcat(request,query);
    return request;
  } else {
    fprintf(stderr,"malloc failed!\n");
    exit;
  }

}

char *get(char *query, char *server) {

  char *request = build_url(server, query);

  if(curl) {
    curl_easy_setopt(curl, CURLOPT_URL, request);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);

    res = curl_easy_perform(curl);
    printf("%s\n", s.ptr);
    value = s.ptr;

    /* Check for errors */ 
    if(res != CURLE_OK)
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
        curl_easy_strerror(res));

    /* always cleanup */ 
    curl_easy_cleanup(curl);
  }

  return value;
}

void set_headers() {

  if(curl) {
    headers = curl_slist_append(headers, "Accept: application/json");
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, "charsets: utf-8");
  }

}

void ir_strcpy( char *s1, const char *s2, int rb, int re )
{
  while( (rb <= re) && (*s1++ = s2[rb]) ) rb++;
  *s1 = 0;
}

char* put(char *query, char* server, char *json_struct) {
  char *request = build_url(server, query);
  set_headers();
  if(curl) {
    curl_easy_setopt(curl, CURLOPT_URL, request);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST"); /* !!! */
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_struct); /* data goes here */
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);

    res = curl_easy_perform(curl);
    printf("%s\n", s.ptr);
    value = s.ptr;

    /* Check for errors */ 
    if(res != CURLE_OK)
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
        curl_easy_strerror(res));

    /* always cleanup */ 
    curl_easy_cleanup(curl);
    return value;
  } else {

    printf("curl not initialized correctly");

  }

}

int main(int argc, char** argv)
{

  // char *jsonObj = "{ \"index\" : {} }\n{ \"name\" : \"videet\" }\n{ \"index\" : {} }\n{ \"name\" : \"surmeet\" }\n";
  init();
  char *method = argv[1];
  char *query = argv[2];
  char *server = argv[3];
  printf("method: %s\n",method);
  printf("query: %s\n",query);
  printf("server: %s\n",server);
 
  FILE *fptr;
  fptr = fopen("program.txt", "w");
  if(fptr == NULL) {
    printf("Error!");
  }

  fprintf(fptr,"hello");

  if(!strcasecmp(method, "GET")) {
    get(query, server);
  } else if(!strcasecmp(method, "PUT")) {

    // char json_data[strlen(argv[4])];
    // ir_strcpy(json_data, argv[4], 1, strlen(argv[4])-2);
    char *json_data = argv[4];
    printf("data: %s\n",argv[4]);
    printf("data: %s\n",json_data);

    fprintf(fptr,"%s\n%s\n%s\n%s", method, query, server, json_data);
    fclose(fptr);
    put(query, server, json_data);
  } else {
   fprintf(fptr,"%s\n%s\n%s\n", method, query, server);
   printf("%s\n","Please enter correct method");
 }

 deinit();
 return 0;
}