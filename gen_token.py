#!/usr/bin/env python3

import sys

import hashlib
def generate_token(endpoint_id, device_token):
    hash_object = hashlib.sha256(
        bytes(endpoint_id, 'utf-8') + bytes(device_token, 'utf-8')
    )
    hex_dig = hash_object.hexdigest()
    return bytes(hex_dig, 'utf-8')

if __name__ == "__main__":
    endpoint_id, device_token = sys.argv[1], sys.argv[2]
    print("endpoint id: \"{}\"".format(endpoint_id))
    print("device token: \"{}\"".format(device_token))
    print(generate_token(endpoint_id, device_token))
