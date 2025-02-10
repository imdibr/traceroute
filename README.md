# Traceroute Program in C++

## Overview
This is a simple traceroute program implemented in C++. It uses raw sockets to send ICMP Echo Request packets and listens for ICMP Time Exceeded responses from intermediate routers to map the path to a destination.

## Features
- Resolves hostname to IP
- Sends ICMP Echo Requests with increasing TTL
- Captures and displays IP addresses of intermediate routers
- Measures round-trip time for each hop

## Requirements
- A Linux/macOS system!
- Root privileges to execute raw socket operations
- A C++17-compatible compiler

## Installation
1. Clone the repository:
   ```bash
   git clone https://github.com/yourusername/traceroute-cpp.git
   cd traceroute-cpp
   ```
2. Compile the program:
   ```bash
   g++ traceroute.cpp -o traceroute
   ```
3. Run with root privileges:
   ```bash
   sudo ./traceroute <target>
   ```

## Example Usage
[Screenshot 2025-02-10 at 11 32 05 AM](https://github.com/user-attachments/assets/dd3230ae-559a-460f-95ce-b881c31ae745)

```bash
sudo ./traceroute google.com
```


