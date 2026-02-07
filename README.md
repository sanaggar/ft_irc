# ft_irc

<p align="center">
  <img src="https://img.shields.io/badge/Language-C++98-blue?style=for-the-badge&logo=cplusplus" alt="C++98"/>
  <img src="https://img.shields.io/badge/Norm-42-00babc?style=for-the-badge" alt="42"/>
  <img src="https://img.shields.io/badge/Networking-TCP/IP-green?style=for-the-badge" alt="TCP/IP"/>
</p>

<p align="center">
  <strong>A fully functional IRC server built from scratch in C++98</strong><br>
  Real-time multi-client chat server implementing the IRC protocol with channels, operators, and authentication
</p>

---

## About The Project

This project is a complete implementation of an **IRC (Internet Relay Chat) server**, following the RFC 2812 standard. It handles multiple simultaneous client connections using non-blocking I/O and `poll()`, allowing users to connect, authenticate, join channels, exchange messages, and manage channel permissions in real time.

The server is compatible with standard IRC clients and supports all core IRC operations including private messaging, channel management, operator privileges, and channel modes.

### Key Concepts Demonstrated

- **Network Programming** — TCP socket creation, binding, listening, and non-blocking I/O
- **Event-Driven Architecture** — Asynchronous multiplexing with `poll()` for scalable client handling
- **Protocol Implementation** — Parsing and responding to IRC protocol commands with proper numeric codes
- **Client-Server Model** — Multi-client management with authentication state machine
- **Object-Oriented Design** — Clean C++98 architecture with Server, User, and Channel classes

---

## Technical Implementation

### Architecture

```
                        ┌──────────────────────────┐
                        │      IRC Clients         │
                        │  (irssi, netcat, etc.)   │
                        └────────┬─────────────────┘
                                 │ TCP connections
                        ┌────────▼─────────────────┐
                        │      poll() loop         │
                        │   (event multiplexer)    │
                        └────────┬─────────────────┘
                                 │
               ┌─────────────────┼─────────────────────┐
               │                 │                     │
     ┌─────────▼──────┐  ┌──────▼───────┐  ┌──────────▼──────┐
     │  New Connection │  │ Client Data  │  │  Disconnection  │
     │    Handler      │  │   Handler    │  │    Cleanup      │
     └─────────┬──────┘  └──────┬───────┘  └─────────────────┘
               │                │
     ┌─────────▼──────┐  ┌─────▼────────────────┐
     │ Authentication │  │   Command Router      │
     │  PASS→NICK→USER│  │                       │
     └────────────────┘  │ JOIN  PART  PRIVMSG   │
                         │ KICK  MODE  TOPIC     │
                         │ INVITE NICK  PING     │
                         └───────────────────────┘
```

### Connection & Authentication Flow

New clients go through a **3-step authentication state machine** before gaining full access:

```
Connect → PASS (password) → NICK (nickname) → USER (identity) → Authenticated
```

Each step is validated independently, and the user is only created once all three commands succeed.

### Channel Mode System

| Mode | Flag | Description |
|------|------|-------------|
| Invite-only | `+i` | Only invited users can join |
| Topic lock | `+t` | Only operators can change the topic |
| Key | `+k` | Password required to join the channel |
| Operator | `+o` | Grant/revoke operator status to a user |
| User limit | `+l` | Set maximum number of users in the channel |

### IRC Commands Supported

| Command | Description |
|---------|-------------|
| `PASS` | Authenticate with the server password |
| `NICK` | Set or change nickname |
| `USER` | Set username and real name |
| `JOIN` | Join one or more channels |
| `PART` | Leave one or more channels |
| `PRIVMSG` | Send a message to a user or channel |
| `TOPIC` | View or set a channel's topic |
| `MODE` | Change channel or user modes |
| `KICK` | Remove a user from a channel |
| `INVITE` | Invite a user to a channel |
| `OPER` | Gain IRC operator privileges |
| `PING/PONG` | Keep-alive mechanism |

---

## Getting Started

### Prerequisites

- C++ compiler with C++98 support
- POSIX-compliant system (Linux/macOS)
- Make

### Installation

```bash
git clone https://github.com/sanaggar/ft_irc.git
cd ft_irc
make
```

### Usage

```bash
./ircserv <port> <password>
```

| Argument | Description |
|----------|-------------|
| `port` | Port number the server listens on |
| `password` | Server connection password |

### Connecting with a Client

**Using netcat:**
```bash
nc localhost 6667
PASS 1234
NICK alice
USER alice 0 * :Alice Wonderland
JOIN #general
PRIVMSG #general :Hello everyone!
```

**Using irssi:**
```bash
irssi -c localhost -p 6667 -w 1234
```

### Example Session

```
# Terminal 1 — Start the server
./ircserv 6667 1234

# Terminal 2 — Connect as Alice
nc localhost 6667
PASS 1234
NICK alice
USER alice 0 * :Alice
JOIN #42
PRIVMSG #42 :Hey!

# Terminal 3 — Connect as Bob
nc localhost 6667
PASS 1234
NICK bob
USER bob 0 * :Bob
JOIN #42
PRIVMSG #42 :Hi Alice!
```

---

## Project Structure

```
ft_irc/
├── Server.hpp / .cpp        # Core server: socket, poll loop, command routing
├── User.hpp / .cpp          # Client representation (nick, auth, channels)
├── Channel.hpp / .cpp       # Channel management (users, ops, modes, topic)
├── main.cpp                 # Entry point, signal handling, main loop
├── serverValidate.cpp       # PASS / NICK / USER authentication logic
├── functions.cpp            # Utility functions (validation, parsing)
├── send_codes.cpp           # IRC numeric replies (001–484) & broadcast
├── defines.hpp              # Constants, macros, IRC response formats
├── commands/
│   ├── join.cpp             # JOIN — channel creation & joining logic
│   ├── part.cpp             # PART — leave channels
│   ├── privmsg.cpp          # PRIVMSG — private & channel messaging
│   ├── nick.cpp             # NICK — nickname changes
│   ├── topic.cpp            # TOPIC — view/set channel topic
│   ├── mode.cpp             # MODE & OPER — channel modes & operator
│   ├── kick.cpp             # KICK — remove users from channels
│   ├── invit.cpp            # INVITE — invite users to channels
│   └── pong.cpp             # PING/PONG — keep-alive
└── Makefile                 # Build configuration
```

---

## Skills Demonstrated

<table>
<tr>
<td width="50%">

**Network & Systems Programming**
- TCP socket programming
- Non-blocking I/O with `poll()`
- Multi-client connection management
- Signal handling (`SIGINT`)
- Buffer management for partial reads

</td>
<td width="50%">

**Software Engineering**
- IRC protocol implementation (RFC 2812)
- Object-oriented C++98 design
- Authentication state machine
- Error handling with standard IRC numeric codes
- Modular command architecture

</td>
</tr>
</table>

---

## Build

```bash
make          # Compile
make clean    # Remove object files
make fclean   # Remove all generated files
make re       # Rebuild
```

**Compiler flags**: `-Wall -Wextra -Werror -std=c++98`

---

## Authors

**sanaggar** · **nstoutze** · **ncardozo** — 42 Nice

---

<p align="center">
  <sub>Made with C++ and sockets at 42</sub>
</p>
