#!/usr/bin/python3
from http.server import HTTPServer, CGIHTTPRequestHandler
"""
服务器端口
"""
port = 8000

httpd = HTTPServer(('', port), CGIHTTPRequestHandler)
print("Starting simple_httpd on port: " + str(httpd.server_port))
httpd.serve_forever()

