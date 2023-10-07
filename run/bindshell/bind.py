import socket  
import sys  

SIZE = 1000

try :  
    host = sys.argv[1]  
    port = sys.argv[2]
except :     
    host = "127.0.0.1"  
    port = 43659

try :  
    sockfd = socket.socket(socket.AF_INET , socket.SOCK_STREAM)

except socket.error as e :  
    
    print("Error while Creating socket : ",e)      
    sys.exit(1)  

try :  
    sockfd.connect((host,port))

except socket.gaierror as e :  

    print("Error (Address-Related) while Connecting to server : ",e)  

except socket.error as e :      

    print("Error while Connecting to Server : ",e)
    sys.exit(1)  

print("BINDSHELL")
try :  
    while 1:  
        cmd = input("(command) $ ").encode('utf-8') + b'\n'  
        sockfd.send(cmd)  
        result = sockfd.recv(SIZE).strip()  
        if not len(result) :  
            sockfd.close()  
            break  
        print(result.decode('utf-8'))  
except KeyboardInterrupt : 
    sockfd.shutdown(0)    
    print("\nBYE")