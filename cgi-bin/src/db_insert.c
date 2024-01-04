#include <curl/curl.h>
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __GNUC__
#define UNUSED(x) UNUSED_ ## x __attribute__((unused))
#else
#define UNUSED(x) x
#endif

#define BUF_CAP 1024

static int callback(void *, int, char**, char**) __attribute__ ((unused));
void replace_plus(char **);

static int callback(void *UNUSED(unused), int argc, char **argv, char **az_col_name) {
  int i;
  for (i = 0; i < argc; i++) {
    printf("%s = %s<br>", az_col_name[i], argv[i] ? argv[i] : "NULL");
  }
  printf("<br>");
  return 0;
}

void replace_plus(char **s) {
  size_t len = strlen(*s);
  for (size_t i = 0; i < len; i++) {
    if ((*s)[i] == '+') {
      (*s)[i] = ' ';
    }
  }
}

int main() {
#ifdef PROD
	printf("Content-Type: text/html\r\n");
  printf("Cache-Control: no-cache, no-store, must-revalidate\r\n");
  printf("X-Frame-Options: DENY\r\n");
  printf("Referrer-Policy: no-referrer\r\n");
  printf("X-Content-Type-Options: nosniff\r\n\r\n");

  char stdin_buf[BUF_CAP];
  size_t input_sz = fread(stdin_buf, sizeof(char), BUF_CAP, stdin);
  char *content = malloc(sizeof(char) * input_sz + 1);

  strncpy(content, stdin_buf, input_sz);
  content[input_sz] = 0x00;

  char db_path[] = "/var/mymoviedb/movies.db";
  sqlite3 *db;
  char *err_msg = 0;
  (void)err_msg;
  int rc;

  if ( (rc = sqlite3_open(db_path, &db)) != 0) {
    fprintf(stderr, "cannot open database: %s<br>", sqlite3_errmsg(db));
    sqlite3_close(db);
    return 1;
  }

  char *int_sep = "=";
  char *ext_sep = "&";
  char *ext_saveptr, *param_value; 

  char *param_key = strtok_r(content, ext_sep, &ext_saveptr);
  char *value;

  char *column_def = malloc((sizeof(char) * BUF_CAP) + 1);
  size_t column_def_sz = 0;
  char *values_def = malloc((sizeof(char) * BUF_CAP) + 1);
  size_t values_def_sz = 0;

  CURL *curl = curl_easy_init();
  if (!curl) {
    fprintf(stderr, "[ERROR] could not init libcurl");
    return 1;
  }
  int decode_len;
  char *decoded;

  while (param_key != NULL) {
    value = strtok_r(param_key, int_sep, &param_value);
    while (value != NULL) {
      decoded = curl_easy_unescape(curl,
                                   param_value,
                                   strlen(param_value),
                                   &decode_len); 

      if (*param_value != '\0' && decoded) {
        strncpy(column_def + column_def_sz, param_key, strlen(param_key));
        column_def_sz += strlen(param_key);
        strncpy(column_def + column_def_sz, ",", 1);
        column_def_sz++;

        strncpy(values_def + values_def_sz, "\"", 1);
        values_def_sz++;
        strncpy(values_def + values_def_sz, decoded, strlen(decoded));
        values_def_sz += strlen(decoded);
        strncpy(values_def + values_def_sz, "\"", 1);
        values_def_sz++;
        strncpy(values_def + values_def_sz, ",", 1);
        values_def_sz++;
      }
      value = strtok_r(NULL, int_sep, &param_value);
    }
    param_key = strtok_r(NULL, ext_sep, &ext_saveptr);
  }
  curl_free(decoded);
  curl_easy_cleanup(curl);

  values_def[values_def_sz - 1] = '\0';
  column_def[column_def_sz - 1] = '\0';

  replace_plus(&values_def);

  char *stmt = malloc((sizeof(char) * BUF_CAP) + 1);
  sprintf(stmt,
          "insert into movies (%s) values (%s);",
          column_def,
          values_def);

  sqlite3_stmt *pp_stmt;
  const char *pz_tail;
  if ( (rc = sqlite3_prepare_v2(db,
                                stmt,
                                strlen(stmt),
                                &pp_stmt,
                                &pz_tail)) != SQLITE_OK) {
    fprintf(stderr, "sql error: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    return 1;
  }

  if ( (rc = sqlite3_step(pp_stmt)) != SQLITE_DONE) {
    fprintf(stderr, "error: %s\n", sqlite3_errmsg(db));
  }

  sqlite3_reset(pp_stmt);
  sqlite3_finalize(pp_stmt);

  sqlite3_close(db);

  puts("<h1>Insert Success</h1>");
  printf("<a href=\"http://home.local/index.html\">Home</a><br>");
  printf("<footer>sqlite3 version: %s</footer>", sqlite3_libversion());

  free(stmt);
  free(values_def);
  free(column_def);
  free(content);
#else
  puts("feature implemented in prod");
#endif
	return 0;
}
