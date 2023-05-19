import json

def verify_user(username,password):
    f = open('users.json')
    data = json.load(f)

    if username in data and data[username] == password:
        return True
    else:
        return False
