# Application Traffic Forwarding via Proxy

This C++ tool captures all network traffic from a specific application and forwards it to a proxy, simulating VPN-like behavior. The proxy routes traffic to the target destination, ensuring all communications from the application are tunneled through the proxy server.

## Features
- Capture outbound traffic from a specified application by Process ID (PID)
- Forward traffic to a designated proxy server (acting as a VPN)
- Relay responses from the proxy back to the application

## Requirements
- **Windows OS**
- **Administrator Privileges**: Required to intercept and modify network traffic
- **WinDivert Library**: Used for packet capture and modification
- **C++ Compiler**: Visual Studio or another compiler supporting C++ on Windows

## Setup

1. **Clone the Repository**:
    ```
    git clone https://github.com/yourusername/proxy-forwarding-tool.git
    cd proxy-forwarding-tool
    ```

2. **Install WinDivert**:
   - Download [WinDivert](https://reqrypt.org/windivert.html) and extract the files.
   - Place `WinDivert.dll` and `WinDivert.lib` in your project directory or in a location accessible by your compiler.

3. **Configure the Tool**:
   - Open `main.cpp` and update the following values:
     - `proxy_ip`: Set to your proxy server’s IP.
     - `proxy_port`: Set to your proxy server’s port.
     - `target_pid`: Set to the PID of the application you wish to capture.

4. **Compile the Program**:
   - Open your C++ compiler (e.g., Visual Studio) and add `WinDivert.lib` as a dependency.
   - Build the project.

## Usage

1. **Run the Application**:
   - Ensure you run the application with **Administrator Privileges**.
   - The tool will start capturing packets from the target application’s PID and forwarding them through the configured proxy.

2. **Test and Verify**:
   - To verify, monitor traffic on the proxy server to confirm it’s receiving and routing the application’s traffic.

## Example Code

This tool is built using C++ and WinDivert. Here’s an example snippet for configuring the tool:

```cpp
const char* proxy_ip = "proxy_ip_address";  // Replace with your proxy IP
const uint16_t proxy_port = 8080;           // Replace with your proxy port
const uint32_t target_pid = 1234;           // Replace with your target application's PID
