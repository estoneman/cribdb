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

  puts("<!DOCTYPE html>");
  puts("<html>");
  puts("<head>");
  puts("<title>Results</title>");
  puts("<link rel=\"stylesheet\" href=\"http://home.local/styles.css\">");
  puts("</head>");

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

  char *stmt = malloc(BUF_CAP + 1);
  strcpy(stmt, "select * from movies;");

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

  printf("<h1>Movies</h1>");

  printf("<table>");
  printf("<tr>"
  "<th>Name</th>"
  "<th>Release</th>"
  "<th>Score</th>"
  "<th>Rating</th>"
  "<th>Cast</th>"
  "</tr>");
  while ( (rc = sqlite3_step(pp_stmt)) != SQLITE_DONE) {
    printf("<tr><td>%s</td><td>%s</td><td>%s</td><td>%s</td>"
    "<td>%s</td></tr>",
           sqlite3_column_text(pp_stmt, 1),
           sqlite3_column_text(pp_stmt, 2),
           sqlite3_column_text(pp_stmt, 3),
           sqlite3_column_text(pp_stmt, 4),
           sqlite3_column_text(pp_stmt, 5));
  }
  printf("</table>");

  sqlite3_reset(pp_stmt);
  sqlite3_finalize(pp_stmt);

  sqlite3_close(db);

  printf("<a href=\"http://home.local/index.html\">Home</a><br>");
  printf("<footer>sqlite3 version: %s</footer>", sqlite3_libversion());

  free(stmt);

	return 0;
}
