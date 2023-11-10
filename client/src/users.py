import bcrypt

def createUser(username, password):
    try:
        hashedPassword, salt = hashPassword(password)
        if hashedPassword == False or salt == False:
            return Exception("Error hashing password")

        with open("users.txt", "a") as f:
            f.write(username + ":" + hashedPassword + "\n")
        return True
    except Exception as e:
        print(e)
        return False

def hashPassword(password, salt=None):
    try:
        if salt == None:
            salt = bcrypt.gensalt()
        hashedPassword = bcrypt.hashpw(password.encode(), salt)
        return hashedPassword.decode(), salt
    except Exception as e:
        print(e)
        return False, False
