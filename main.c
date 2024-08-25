#include <stdio.h>

int ftp_download(const char *url, const char *local_file);
int ftp_upload_and_rename(const char *local_file, const char *remote_url, const char *rename_file_to);

int main(void) {
    const char *download_url = "ftp://user:password@192.168.2.39/Desktop/lecture/demo_server/info"; /* URL of the file to be downloaded from the FTP server */
    const char *downloaded_file = "/home/user/Desktop/lecture/demo_client/info1"; /* Name of the file where the downloaded content will be saved locally */

    const char *upload_local_file = "/home/user/Desktop/lecture/demo_client/example"; /* Path to the local file that will be uploaded to the FTP server */
    const char *upload_remote_url = "ftp://user:password@192.168.2.39/Desktop/lecture/demo_server/example1"; /* Remote URL on the FTP server where the file will be uploaded */
    const char *rename_to = "example2"; /* Name to rename the uploaded file to, after the upload is complete on the FTP server */

    /* Download the file */
    if (ftp_download(download_url, downloaded_file) != 0) {
        fprintf(stderr, "Error downloading file.\n");
        return 1;
    }

    /* Upload the file and rename it */
    if (ftp_upload_and_rename(upload_local_file, upload_remote_url, rename_to) != 0) {
        fprintf(stderr, "Error uploading file.\n");
        return 1;
    }

    printf("Download and upload operations completed successfully.\n");
    return 0;
}

