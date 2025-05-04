#!/usr/bin/env python3

import http.server
import ssl

PORT = 5555
HOST = "127.0.0.1"

handler = http.server.SimpleHTTPRequestHandler
httpd = http.server.HTTPServer((HOST, PORT), handler)

context = ssl.SSLContext(ssl.PROTOCOL_TLS_SERVER)
context.load_cert_chain(certfile="server.crt", keyfile="server.key")

httpd.socket = context.wrap_socket(httpd.socket, server_side=True)

print(f"Serving HTTPS on https://{HOST}:{PORT}")
httpd.serve_forever()

