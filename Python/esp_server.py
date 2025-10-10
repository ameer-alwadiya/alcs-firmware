import socket

# Create server socket
server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server.bind(('0.0.0.0', 8080))  # Listen on all interfaces, port 8080
server.listen(1)

print(f"Server listening on 192.168.178.77:8080")
print("Waiting for ESP01 connections...")

while True:
    client, addr = server.accept()
    print(f"Got connection from {addr}")
    
    # Receive data
    data = client.recv(1024)
    if data:
        print(f"Received: {data.decode('utf-8', errors='ignore')}")
        
        # Send response
        client.send(b"Message received OK\r\n")
    
    client.close()
    print("Connection closed\n")