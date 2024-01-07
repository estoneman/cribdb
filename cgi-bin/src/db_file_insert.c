#include <curl/curl.h>
#include <errno.h>
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __GNUC__
#define UNUSED(x) UNUSED_ ## x __attribute__((unused))
#else
#define UNUSED(x) x
#endif

#ifdef PROD
#define DB_PATH "/var/mymoviedb/movies.db"
#else
#define DB_PATH "./db/movies.db"
#endif

static int callback(void *, int, char**, char**) __attribute__ ((unused));

static int callback(void *UNUSED(unused), int argc, char **argv, char **az_col_name) {
  int i;
  for (i = 0; i < argc; i++) {
    printf("%s = %s<br>", az_col_name[i], argv[i] ? argv[i] : "NULL");
  }
  printf("<br>");
  return 0;
}

int main() {
	printf("Content-Type: text/html\r\n");
  printf("Cache-Control: no-cache, no-store, must-revalidate\r\n");
  printf("X-Frame-Options: DENY\r\n");
  printf("Referrer-Policy: no-referrer\r\n");
  printf("X-Content-Type-Options: nosniff\r\n\r\n");

  int content_length = (int)strtol(getenv("CONTENT_LENGTH"), (char**)NULL, 10);
  char *content = malloc(sizeof(char) * content_length + 1);
  size_t nb_read = fread(content, sizeof(char), content_length, stdin);

  content[nb_read] = 0x00;

  const char *filename = "upload.csv";
  FILE *fp = fopen(filename, "w");
  if (fp == NULL) {
    fprintf(stderr, "[ERROR] could not open %s: %s\n", filename, strerror(errno));
    return 1;
  }
  size_t nb_written = fwrite(content, 1, nb_read, fp);
  if (nb_written != nb_read) {
    fprintf(stderr, "[ERROR] only wrote %zu bytes to %s\n", nb_written, filename);
    return 1;
  }
  fclose(fp);

  sqlite3 *db;
  char *err_msg = 0;
  (void)err_msg;
  int rc;

  if ( (rc = sqlite3_open(DB_PATH, &db)) != SQLITE_OK) {
    fprintf(stderr, "cannot open database: %s<br>", sqlite3_errmsg(db));
    sqlite3_close(db);
    return 1;
  }

  free(content);
  sqlite3_close(db);

	return 0;
}
