from http.server import BaseHTTPRequestHandler, HTTPServer
import json

class Handler(BaseHTTPRequestHandler):
    def do_POST(self):
        length = int(self.headers['Content-Length'])
        data = self.rfile.read(length)
        print("\n=== Webhook Received ===")
        print(data.decode())
        self.send_response(200)
        self.end_headers()

HTTPServer(('', 5000), Handler).serve_forever()