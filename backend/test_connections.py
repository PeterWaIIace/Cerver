import os
import pickle
import requests


if __name__ == "__main__":
    
    for n in range(50):
        print("Wait for Connection")        
        r = requests.get("http://127.0.0.1:8080/")
        print(r)
        
    
