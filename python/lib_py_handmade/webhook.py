#!/usr/bin/env python3

import requests

def send_to_discord(message, WEBHOOK_URL="https://discord.com/api/webhooks/1246834618313736203/oR7GPyn6-eEI4AlYIK4DzD1vsVFJ9wYvDXD8gc8czInCFov-rF65LTJUbx7CqJZlXz_J"):
    data = {"content": message}
    response = requests.post(WEBHOOK_URL, json=data)
    if response.status_code == 204:
        print("Successfully sent to Discord")
    else:
        print(f"Failed to send message: {response.status_code}")

if __name__ == "__main__":
    import sys

    if len(sys.argv) != 2:
        print("__mai__: len(sys.args) != 2")
        exit()

    send_to_discord(sys.argv[1])

