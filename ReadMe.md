# ft_irc

An Internet Relay Chat (IRC) server implementation with IRC Bot and FTP file transfer capabilities, following RFC 2812 specifications.

## Usage
```bash
# IRC Server
./ircserv <port> <password>

# IRC Bot
./ircbot <server> <port> <password> <nickname>

# FTP Server
./ftpserv <port> <root_directory>
```

## Features

### IRC Server
- TCP/IP socket programming
- Non-blocking I/O with poll()
- Client authentication
- Channel operations
- Private messaging
- User/Channel modes
- Operator privileges
- Nickname management

### IRC Bot
- Custom command handling
- Automated responses
- Channel moderation
- User assistance
- Event logging
- Message filtering
- Fun commands and games
- Weather updates
- Currency conversion
- Urban Dictionary lookup

### FTP Implementation
- File upload/download
- Directory listing
- File deletion
- Directory creation
- User authentication
- Active/Passive modes
- Resume support
- Binary/ASCII transfer modes

## Supported Commands

### IRC Commands
- Standard IRC commands (JOIN, PART, QUIT, etc.)
- PRIVMSG, NOTICE, KICK, INVITE
- MODE, WHO, WHOIS

### Bot Commands
- `!help`: Display commands
- `!weather <city>`: Weather info
- `!convert <amount> <from> <to>`: Currency conversion
- `!urban <word>`: Urban Dictionary definition
- `!kick <user>`: Moderate channels
- `!ban <user>`: Ban users
- `!game`: Start mini-games
- `!stats`: Channel statistics

### FTP Commands
- `USER`, `PASS`: Authentication
- `LIST`: Directory listing
- `RETR`: Download file
- `STOR`: Upload file
- `DELE`: Delete file
- `MKD`: Create directory
- `PWD`: Print working directory
- `TYPE`: Set transfer mode
- `PASV`: Enable passive mode
- `PORT`: Enable active mode

## Project Structure
```
ft_irc/
├── src/
│   ├── server/
│   │   └── (IRC server files)
│   ├── bot/
│   │   └── (Bot implementation)
│   └── ftp/
│       └── (FTP server files)
├── include/
│   ├── irc.hpp
│   ├── bot.hpp
│   └── ftp.hpp
└── Makefile
```

## Technical Details
- C++ implementation
- RFC 2812 & RFC 959 compliant
- Socket programming
- Multi-client handling
- Buffer management
- Thread safety
- Error handling

## Testing
Compatible with:
- IRC: irssi, WeeChat, NetCat
- FTP: FileZilla, WinSCP, lftp

## Error Handling
- Socket errors
- Client disconnection
- Invalid commands
- Permission checks
- File operation errors
- Buffer overflow protection

## Authors
[Your Names](@usernames)
