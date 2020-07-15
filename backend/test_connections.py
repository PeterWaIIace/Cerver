import os
import pickle
import requests
import threading
import time

def thread_function():
    print("Wait for Connection")        
    r = requests.get("http://127.0.0.1:8080/")
    print(r)


if __name__ == "__main__":

    x=[]
    m=100
    for n in range(m):
        x.append(threading.Thread(target=thread_function, args=()))
    
    for n in range(m):
        x[n].start()
    
    for n in range(m):
        x[n].join()

    