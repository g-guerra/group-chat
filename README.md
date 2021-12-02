# Group Chat

This is a group chat application implemented in C. It is a client/server application based on TCP/IP.

## Pre-requisites
* Must be compiled and run on Linux

## Installation

Download the source code to the target folder with:
```bash
git clone https://github.com/g-guerra/group-chat.git
```

To compile the server do the following commands from the project root folder:
```bash
cd Server
make
```

To compile the client do the following commands from the project root folder:
```bash
cd Server
make
```

To clean the server compilation output do the following commands from the project root folder:
```bash
cd Server
make clean
```

To clean the client compilation output do the following commands from the project root folder:
```bash
cd Client
make clean
```

## Usage

To run the server do the following command from the project root folder:
```bash
cd Server
./server.elf
```

To run the client do the following command from the project root folder:
```bash
cd Client
./client.elf "<message>"
```

To send messages from the client just type in the client application the message and press Enter.
To quit the client application press CTRL+C or send exit.
To quit the server application press CTRL+C.

## Features

* The server IP address and Port are hard coded.
* The server handles up to 100 clients.
* A message is broadcasted to everyone except the sender.
* The server detects when a client is AFK.

## Support
### Francisco Marques
fmarques_00@protonmail.com
### Gonçalo Guerra
guerra.m.goncalo@protonmail.ch
