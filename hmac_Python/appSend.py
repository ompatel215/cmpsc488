import hmac
import hashlib
import json

SECRET_KEY = b'password'

def generate_hmac(message: str, key: bytes):
    return hmac.new(key, message.encode(), hashlib.sha256).hexdigest()

def send_payload():
    payload = {"message": "Hello, Om!"}
    payload_str = json.dumps(payload)
    signature = generate_hmac(payload_str, SECRET_KEY)
    data = {"payload": payload, "signature": signature}
    
    with open("data.json", "w") as f:
        json.dump(data, f)

if __name__ == "__main__":
    send_payload()
