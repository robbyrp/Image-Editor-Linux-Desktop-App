/* Based on the libcurl getinmemory example:
 * https://curl.se/libcurl/c/getinmemory.html
 * Modified for this project.
 */
#include "network.h"
#include <stdlib.h>
#include <string.h>

int network_init(void)
{
    CURLcode result;
    result = curl_global_init(CURL_GLOBAL_ALL);
    if (result != CURLE_OK) return (int)result;
    return 0;
}

void network_cleanup(void)
{
    curl_global_cleanup();
}

static size_t write_cb(char *contents, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;
    memory_struct_t *mem = (memory_struct_t *)userp;

    char *ptr = realloc(mem->memory, mem->size + realsize + 1);
    if(!ptr) {
        /* out of memory! */
        printf("not enough memory (realloc returned NULL)\n");
        return 0;
    }

    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}

void free_chunk(memory_struct_t *chunk) {
    free(chunk->memory);
    chunk->memory = NULL;
    chunk->size = 0;
}
memory_struct_t http_get_request_content(const char *route) 
{
    memory_struct_t chunk;

    chunk.memory = malloc(1); /* grown as needed by the realloc above */
    chunk.size = 0;           /* no data at this point */

    /* init the curl session */
    CURL *curl = curl_easy_init();
    if (!curl) {
        free_chunk(&chunk);
        return chunk;
    }

    char full_url[512];
    snprintf(full_url, sizeof(full_url), "%s%s", BACKEND_BASE_URL, route);

    /* specify URL to get */
    curl_easy_setopt(curl, CURLOPT_URL, full_url);

    /* send all data to this function */
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);

    /* we pass our 'chunk' struct to the callback function */
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

    /* some servers do not like requests that are made without a user-agent
        field, so we provide one */
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");

    /* get it! */
    CURLcode result = curl_easy_perform(curl);

    /* check for errors */
    if(result != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(result));
        free_chunk(&chunk);
    } else {
        /*
        * Now, our chunk.memory points to a memory block that is chunk.size
        * bytes big and contains the remote file.
        *
        * Do something nice with it!
        */
        printf("%lu bytes retrieved\n", (unsigned long)chunk.size);
    }

    curl_easy_cleanup(curl);
    return chunk;
    
}
