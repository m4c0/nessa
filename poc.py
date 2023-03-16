#!/usr/bin/python3
from http.server import HTTPServer, SimpleHTTPRequestHandler, test
import os, sys, logging
import argparse

# This is required by WebKit for shared-memory
class CORSRequestHandler (SimpleHTTPRequestHandler):
    def end_headers (self):
        self.send_header('Cross-Origin-Opener-Policy', 'same-origin')
        self.send_header('Cross-Origin-Embedder-Policy', 'require-corp')
        SimpleHTTPRequestHandler.end_headers(self)

if __name__ == '__main__':
    test(CORSRequestHandler, HTTPServer)
