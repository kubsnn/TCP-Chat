from cryptography.hazmat.primitives.asymmetric import rsa
from cryptography.hazmat.primitives import serialization
from cryptography.hazmat.primitives import hashes
from cryptography.hazmat.primitives.asymmetric import padding
from cryptography.hazmat.primitives.asymmetric import dh

def generateSenderPrivateKey():
    try:
        return rsa.generate_private_key(
            public_exponent=65537,
            key_size=2048,
        )
    except Exception as e:
        print(e)
        return False

def generateSenderPublicKey(privateKey):
    try:
        return privateKey.public_key()
    except Exception as e:
        print(e)
        return False

def generateSenderPrivatePEM(privateKey):
    try:
        return privateKey.private_bytes(
            encoding=serialization.Encoding.PEM,
            format=serialization.PrivateFormat.PKCS8,
            encryption_algorithm=serialization.NoEncryption(),
        )
    except Exception as e:
        print(e)
        return False

def generateSenderPublicPEM(publicKey):
    try:
        return publicKey.public_bytes(
            encoding=serialization.Encoding.PEM,
            format=serialization.PublicFormat.SubjectPublicKeyInfo,
        )
    except Exception as e:
        print(e)
        return False

def messageEncryption(message, publicKey):
    try:
        return publicKey.encrypt(
            message.encode(),
            padding=padding.OAEP(
                mgf=padding.MGF1(algorithm=hashes.SHA256()),
                algorithm=hashes.SHA256(),
                label=None,
            )
        )
    except Exception as e:
        print(e)

def messageDecryption(encryptedMessage, privateKey):
    try:
        return privateKey.decrypt(
            encryptedMessage,
            padding=padding.OAEP(
                mgf=padding.MGF1(algorithm=hashes.SHA256()),
                algorithm=hashes.SHA256(),
                label=None,
            )
        ).decode()
    except Exception as e:
        print(e)
        return False