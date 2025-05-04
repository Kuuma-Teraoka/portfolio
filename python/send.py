#!/usr/bin/env python3

import webhook

import sys

if len(sys.argv) != 2:
    print("__mai__: len(sys.args) != 2")
    exit()

webhook.send_to_discord(sys.argv[1])

