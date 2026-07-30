#ifndef NETWORK_H
#define NETWORK_H

#include <curl/curl.h>
#define BACKEND_BASE_URL "http://127.0.0.1:8000"

#ifdef __cplusplus
extern "C" {
#endif
    typedef struct {
        char *memory;
        size_t size;
	} memory_struct_t;

    int network_init(void);
	memory_struct_t http_get_request_content(const char *route);
	void free_chunk(memory_struct_t *chunk);
	void network_cleanup(void);

#ifdef __cplusplus
}
#endif

#endif