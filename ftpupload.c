/* Performs an FTP upload and renames the file just after a successful transfer. */

#define _POSIX_C_SOURCE 200112L

#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

static size_t read_callback(char *ptr, size_t size, size_t nmemb, void *stream) {
    unsigned long nread;
    /* in real-world cases, this would probably get this data differently
     as this fread() stuff is exactly what the library already would do
     by default internally */
    size_t retcode = fread(ptr, size, nmemb, stream);
    if(retcode > 0) {
        nread = (unsigned long)retcode;
        fprintf(stderr, "*** We read %lu bytes from file\n", nread);
    }
    return retcode;
}

/* Function to upload a file to an FTP server and rename it */
int ftp_upload_and_rename(const char *local_file, const char *remote_url, const char *rename_file_to) {
    CURL *curl;
    CURLcode res;
    FILE *hd_src;
    struct stat file_info;
    unsigned long fsize;

    struct curl_slist *headerlist = NULL;
    char buf_1[256];
    char buf_2[256];

    /* get the file size of the local file */
    if(stat(local_file, &file_info)) {
        printf("Couldn't open '%s': %s\n", local_file, strerror(errno));
        return 1;
    }
    fsize = (unsigned long)file_info.st_size;

    printf("Local file size: %lu bytes.\n", fsize);

    /* get a FILE * of the same file */
    hd_src = fopen(local_file, "rb");

    curl_global_init(CURL_GLOBAL_ALL);

    /* get a curl handle */
    curl = curl_easy_init();
    if(curl) {
        snprintf(buf_1, sizeof(buf_1), "RNFR %s", strrchr(remote_url, '/') + 1);
        snprintf(buf_2, sizeof(buf_2), "RNTO %s", rename_file_to);

        /* build a list of commands to pass to libcurl */
        headerlist = curl_slist_append(headerlist, buf_1);
        headerlist = curl_slist_append(headerlist, buf_2);

        /* we want to use our own read function */
        curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_callback);

        /* enable uploading */
        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

        /* specify target */
        curl_easy_setopt(curl, CURLOPT_URL, remote_url);

        /* pass in that last of FTP commands to run after the transfer */
        curl_easy_setopt(curl, CURLOPT_POSTQUOTE, headerlist);

        /* now specify which file to upload */
        curl_easy_setopt(curl, CURLOPT_READDATA, hd_src);

        curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t) fsize);

        /* Now run off and do what you have been told! */
        res = curl_easy_perform(curl);
        /* Check for errors */
        if (res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                    curl_easy_strerror(res));

        /* clean up the FTP commands list */
        curl_slist_free_all(headerlist);

        /* always cleanup */
        curl_easy_cleanup(curl);
    }
    fclose(hd_src); /* close the local file */

    curl_global_cleanup();
    return (res == CURLE_OK) ? 0 : 1;
}

