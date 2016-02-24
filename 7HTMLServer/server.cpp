static const char* templ = "HTTP/1.0 200 OK\r\n"

		           "Content-length: %d\r\n"

		       	   "Connection: close\r\n"

		       	   "Content-Type: text/html\r\n"

		       	   "\r\n"

		       	   "%s";

static const char not_found[] = "HTTP/1.0 404 NOT FOUND\r\nContent-Type: text/html\r\n\r\n";
