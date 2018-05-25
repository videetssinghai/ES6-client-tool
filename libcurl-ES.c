#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>



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

char *get() {

  if(curl) {

    curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:9200/_bulk");
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
    headers = curl_slist_append(headers, "Content-Type: application/x-ndjson");
    headers = curl_slist_append(headers, "charsets: utf-8");
  }


}

char* post(char* json_struct) {

  set_headers();
  curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:9200/data/name/_bulk");
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

}


char* put(char* json_struct) {

  set_headers();
  curl_easy_setopt(curl, CURLOPT_URL, "http://localst:9200/data/name/_bulk");
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
  curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT"); /* !!! */
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
}

int main(void)
{
  /*"{ \"name\" : \"Pedro\" , \"age\" : \"22\" }" */

  /*char* jsonObj = "{ \"index\" : \"{ \"_index\" : \"sample\", \"type\" : \"s\"  } },\n{ \"Amount\": \"480\", \"Quantity\": \"2\", \"Id\": \"1\", \"Client_Store_sk\": \"1109\"},\n{ \"index\" : \"{ \"_index\" : \"sample\", \"type\" : \"s\" }},\n{\"Amount\": \"23\", \"Quantity\": \"2\", \"Id\": \"2\", \"Client_Store_sk\": \"50\"},\n"; */
  char *jsonObj = "{ \"index\" : {} }\n{ \"name\" : \"videet\" }\n{ \"index\" : {} }\n{ \"name\" : \"surmeet\" }\n";

  init();
  printf("%s\n", jsonObj); 
  put(jsonObj);
  deinit();
  return 0;
}



