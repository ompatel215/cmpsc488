import hmac
import hashlib
import json

SECRET_KEY = b'password'

def verify_hmac(message: str, signature: str, key: bytes):
    expected_signature = hmac.new(key, message.encode(), hashlib.sha256).hexdigest()
    return hmac.compare_digest(expected_signature, signature)

def receive_payload():
    try:
        with open("data.json", "r") as f:
            data = json.load(f)

        payload_str = json.dumps(data["payload"])
        signature = data["signature"]
        
        return verify_hmac(payload_str, signature, SECRET_KEY)
    except FileNotFoundError:
        return False

if __name__ == "__main__":
    receive_payload()
