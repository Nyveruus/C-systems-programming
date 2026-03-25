# HTTP Server

HTTP/1.1 file server in C. it serves static files from the local filesystem over TCP, with support for URL paths and basic MIME types.

The server achitecturally follows a model of accept connection and then thread. In essence, a single main loop accepts incoming TCP connections and dispatches each one to a dedicated POSIX thread which allows concurrency without any blocking.

## Implementation Details

It begins by setting up a signal handler and using a create() function that creates a TCP socket and sockaddr to bind to and listen on. Main enters a while running loop. If SIGINT is received, the loop does not continue, it closes its socket and closes. In the loop it continuously accepts connections, if no file desciptor is returned (no connection), then it just continues on to the next iteration of the loop. If a client indeed connects, then the client file descriptor is returned by accept() and the loop creates a detached POSIX thread to handle the new client.

The handler starts with doing a recv of the client socket into a buffer and proceeds to parsing the request. It compiles a regex to match the GET request itself and a capture group for the file URL. The filename and extension is extracted from the URL using seperate functions which will both used later to build the HTTP response. After the necessary vars are initialized (response, response_len, extension, file) or assigned build_http() is called, passing those vars as pointers. Once the response is fully built, it is sent to the client socket and the thread performs necessary cleanup before closing.

build_http uses the extension to find the MIME type and declares the header. It attempts to open a file descriptor for the requested file. If the file descriptor is unsuccessfully opened, then it frees the header and builds the response directly, returning 404 Not Found. If the file descriptor is succesfully opened, then it prepares the 200 OK header with the MIME type and saves it to the response before reading the file to the response - pointer arithmetic is used to not overwrite the header, i.e. reading to response + *response_len. After building the response, cleanup occurs for heap memory and the function ends.

Important documentation:
- https://www.rfc-editor.org/rfc/rfc2616

## Usage

```

```

## Installation

```

```

## Example

```

```
