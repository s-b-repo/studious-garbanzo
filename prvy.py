import pydivert
import socket

# Configuration
proxy_host = "proxy_ip_address"  # Replace with your proxy IP
proxy_port = 8080                # Replace with your proxy port
target_pid = 1234                # Replace with your target application's PID

def forward_to_proxy(packet):
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        try:
            # Connect to the proxy
            s.connect((proxy_host, proxy_port))
            # Send the packet data to the proxy
            s.sendall(packet.payload)
            # Receive response from the proxy
            response = s.recv(4096)
            # Replace packet payload with proxy response
            packet.payload = response
        except Exception as e:
            print(f"Error forwarding packet: {e}")

def main():
    # Filter traffic by target PID and redirect it through the proxy
    with pydivert.WinDivert(f"ip and tcp and outbound and ip.DstAddr != {proxy_host}") as w:
        print("Capturing packets...")
        for packet in w:
            if packet.process_id == target_pid:
                print(f"Forwarding packet from PID {target_pid} through proxy.")
                forward_to_proxy(packet)
                w.send(packet)  # Send the modified packet back

if __name__ == "__main__":
    main()
