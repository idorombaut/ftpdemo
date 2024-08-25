/* Get a single file from an FTP server. */

#include <stdio.h>
#include <curl/curl.h>

struct FtpFile {
    const char *filename; /* Name of the file to save the downloaded data */
    FILE *stream; /* File stream for the file, initialized to NULL */
};

/* Callback function to write downloaded data to a file */
static size_t my_fwrite(void *buffer, size_t size, size_t nmemb, void *stream) {
    struct FtpFile *out = (struct FtpFile *)stream;
    if(!out->stream) {
        /* open file for writing */
        out->stream = fopen(out->filename, "wb");
        if(!out->stream)
            return 0; /* failure, cannot open file to write */
    }
    return fwrite(buffer, size, nmemb, out->stream);
}

/* Function to download a file from an FTP server */
int ftp_download(const char *url, const char *local_file) {
    CURL *curl;
    CURLcode res;
    struct FtpFile ftpfile = { local_file, NULL };

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        /* Define our callback to get called when there is data to be written */
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, my_fwrite);
        /* Set a pointer to our struct to pass to the callback */
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ftpfile);
        /* Switch on full protocol/debug output */
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

        res = curl_easy_perform(curl);

        /* always cleanup */
        curl_easy_cleanup(curl);

        if (CURLE_OK != res) {
            /* we failed */
            fprintf(stderr, "curl told us %d\n", res);
        }
    }

    if(ftpfile.stream)
        fclose(ftpfile.stream); /* close the local file */

    curl_global_cleanup();
    return (res == CURLE_OK) ? 0 : 1;
}

