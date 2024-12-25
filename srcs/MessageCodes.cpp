#include "../includes/MessageCodes.hpp"

MessageCodes::MessageCodes() {
    _red = MessageCodes::to_string("\e[31m");
    _green = "\e[32m";
    _yellow = "\e[43m";
    _grey = "\e[90m";
    _no_color = "\e[0m";
    _error = -1;
    _svr_name = "HOF";
    _svr_prefix = ":" + _svr_name;
    _max_clients = 1024;
    _port_min = 1024;
    _port_max = 65535;
    _buffer_size = 1024;
    _timeout = 60000;
}

int MessageCodes::getMaxClients() const { return _max_clients; }
int MessageCodes::getError() const { return _error; }
const std::string& MessageCodes::getRed() const { return _red; }
const std::string& MessageCodes::getGreen() const { return _green; }
const std::string& MessageCodes::getYellow() const { return _yellow; }
const std::string& MessageCodes::getGrey() const { return _grey; }
const std::string& MessageCodes::getNoColor() const { return _no_color; }
const std::string& MessageCodes::getSvrPrefix() const { return _svr_prefix; }
const std::string& MessageCodes::getSvrName() const { return _svr_name; }
int MessageCodes::getPortMin() const { return _port_min; }
int MessageCodes::getPortMax() const { return _port_max; }
int MessageCodes::getBufferSize() const { return _buffer_size; }
int MessageCodes::getTimeout() const { return _timeout; }


void MessageCodes::msg_log(const std::string& msg) {
    time_t time_raw;
    tm* timeinfo;
    char buffer[100];

    time(&time_raw);
    timeinfo = localtime(&time_raw);

    strftime(buffer, sizeof(buffer), "%d/%m/%Y %H:%M:%S :", timeinfo);
    std::string time_str(buffer);

    std::cout << _grey << time_str << _no_color << " " << msg << std::endl;
}

// Server
const std::string MessageCodes::msgServerCreated() { return "IRC server created."; }
const std::string MessageCodes::msgServerStart() { return "IRC server starting..."; }
const std::string MessageCodes::msgServerStarted() { return "IRC server started."; }
const std::string MessageCodes::msgServerWelcome() { return "Welcome to this IRC Server!"; }
const std::string MessageCodes::msgServerShutdown() { return "Shutting down IRC server..."; }
const std::string MessageCodes::msgServerEnd() { return "IRC server turned OFF"; }
const std::string MessageCodes::msgServerExitSignal() { return "Exit signal received, program will leave..."; }

// Client
const std::string MessageCodes::msgClientConnected(int socket) { return "A new client has been connected on socket " + MessageCodes::to_string(socket) + "!"; }
const std::string MessageCodes::msgClientDisconnected(int socket) { return "Client on socket " + MessageCodes::to_string(socket) + " has been disconnected."; }
const std::string MessageCodes::msgClientNick(int socket, const std::string& nick) { return "User at socket " + MessageCodes::to_string(socket) + " is now known as " + nick; }
const std::string MessageCodes::msgClientUser(int socket, const std::string& name, const std::string& real) { return "User at socket " + MessageCodes::to_string(socket) + "'s name is " + name + " (" + real + ")"; }
const std::string MessageCodes::msgClientServerShutdown() { return _svr_prefix + " :server now turned OFF."; }
const std::string MessageCodes::msgClientQuit(const std::string& nick) { return _svr_prefix + " " + nick + " :Good by, " + nick + "!"; }

// Dev
const std::string MessageCodes::msgDevPortIsSet() { return "PORT HAS BEEN SET: "; }
const std::string MessageCodes::msgDevServerSocketCreated() { return "SERVER SOCKET CREATED: "; }
const std::string MessageCodes::msgDevServerSocketSetOpt() { return "OPTION SO_REUSEADDR SET ON SERVER SOCKET: "; }
const std::string MessageCodes::msgDevServerSocketNonBlock() { return "SERVER SOCKER SET UP TO NONBLOCK"; }
const std::string MessageCodes::msgDevServerIpSetup() { return "SERVER IP CONFIGURED: "; }
const std::string MessageCodes::msgDevServerSocketIpv4Binded() { return "SERVER SOCKET BINDED TO IPv4 ADDRESS"; }
const std::string MessageCodes::msgDevServerSocketListen() { return "SERVER SOCKET LISTENING MODE ENABLED"; }

// Misc Messages
const std::string MessageCodes::sendNick(const std::string& full, const std::string& nick) { return ":" + full + " NICK :" + nick; }
const std::string MessageCodes::sendInvite(const std::string& full, const std::string& nick, const std::string& chan) { return ":" + full + " INVITE " + nick + " :" + chan; }
const std::string MessageCodes::sendPm(const std::string& from, const std::string& to, const std::string& msg) { return ":" + from + " PRIVMSG " + to + " :" + msg; }
const std::string MessageCodes::sendPong(const std::string& nick) { return _svr_prefix + " PONG " + _svr_name + " :" + nick; }
const std::string MessageCodes::sendPart(const std::string& full, const std::string& chan) { return ":" + full + " PART " + chan; }
const std::string MessageCodes::sendPartMsg(const std::string& full, const std::string& chan, const std::string& msg) { return ":" + full + " PART " + chan + " :" + msg; }
const std::string MessageCodes::sendJoin(const std::string& full, const std::string& chan) { return ":" + full + " JOIN :" + chan; }
const std::string MessageCodes::sendQuit(const std::string& full, const std::string& nick) { return ":" + full + " QUIT :Quit: " + nick; }
const std::string MessageCodes::sendQuitMsg(const std::string& full, const std::string& msg) { return ":" + full + " QUIT :Quit: " + msg; }
const std::string MessageCodes::sendKick(const std::string& full, const std::string& chan, const std::string& target, const std::string& nick) { return ":" + full + " KICK " + chan + " " + target + " :" + nick; }
const std::string MessageCodes::sendModeUser(const std::string& full, const std::string& nick, const std::string& mode) { return ":" + full + " MODE " + nick + " :" + mode; }
const std::string MessageCodes::sendModeChan(const std::string& full, const std::string& chan, const std::string& mode) { return ":" + full + " MODE " + chan + " " + mode; }
const std::string MessageCodes::sendTopic(const std::string& full, const std::string& chan, const std::string& topic) { return ":" + full + " TOPIC " + chan + " :" + topic; }

// Channel
const std::string MessageCodes::msgChannelCreated(const std::string& nick, const std::string& chan) { return "New channel created by " + nick + ": " + chan; }

// irc
const std::string MessageCodes::errSvrUsage(const std::string& prog_name) { return "Usage: " + prog_name + " <port> <password>"; }


// ERR
const std::string MessageCodes::errInvalidUsername(const std::string& username) { return _svr_prefix + " 468 " + username + ": Invalid username"; }
const std::string MessageCodes::errInvalidRealname(const std::string& realname) { return _svr_prefix + " " + realname + ": Invalid realname"; }
const std::string MessageCodes::errBadChannelName(const std::string& nickname, const std::string& wrongname) { return _svr_prefix + " 479 " + nickname + " " + wrongname + " :Invalid channel name."; }
const std::string MessageCodes::errSyntax(const std::string& nickname, const std::string& command) { return _svr_prefix + " " + nickname + " " + command + " :Syntax error."; }

// RPL_MSG_CODE
const std::string MessageCodes::rplWelcome(const std::string& nick, const std::string& full) { return _svr_prefix + " 001 " + nick + " :Welcome to the HOF(Huseyin,Oya,Furkan) IRC server " + full; }
const std::string MessageCodes::rplYourHost(const std::string& nick) { return _svr_prefix + " 002 " + nick + " :You host is " + _svr_name + ", running version 1.0."; }
const std::string MessageCodes::rplCreated(const std::string& nick) { return _svr_prefix + " 003 " + nick + " :This server was created in 2024."; }
const std::string MessageCodes::rplMyInfo(const std::string& nick) { return _svr_prefix + " 004 " + nick + " :" + _svr_name + " v1.0 "; }
const std::string MessageCodes::rplUModeIs(const std::string& nick, const std::string& modes) { return _svr_prefix + " 221 " + nick + " " + modes; }
const std::string MessageCodes::rplChannelModeIs(const std::string& nick, const std::string& chan, const std::string& mode) { return _svr_prefix + " 324 " + nick + " " + chan + " +" + mode; }
const std::string MessageCodes::rplNoTopic(const std::string& nick, const std::string& chan) const { return _svr_prefix + " 331 " + nick + " " + chan + " :No topic is set"; }
const std::string MessageCodes::rplTopic(const std::string& nick, const std::string& chan, const std::string& topic) const { return _svr_prefix + " 332 " + nick + " " + chan + " :" + topic; }
const std::string MessageCodes::rplInviting(const std::string& nick, const std::string& target, const std::string& chan) { return _svr_prefix + " 341 " + nick + " " + target + " " + chan; }
const std::string MessageCodes::rplNameReply(const std::string& nick, const std::string& chan, const std::string& list) { return _svr_prefix + " 353 " + nick + " = " + chan + " :" + list; }
const std::string MessageCodes::rplEndOfNames(const std::string& nick, const std::string& chan) { return _svr_prefix + " 366 " + nick + " " + chan + " :End of /NAMES list."; }
const std::string MessageCodes::rplYoureOper(const std::string& nick) { return _svr_prefix + " 381 " + nick + " :You are now server Operator."; }

// Error Messages
const std::string MessageCodes::errorNoSuchNick(const std::string& nick, const std::string& target) { return _svr_prefix + " 401 " + nick + " " + target + " :No such nick."; }
const std::string MessageCodes::errorNoSuchChannel(const std::string& nick, const std::string& chan) { return _svr_prefix + " 403 " + nick + " " + chan + " :No such channel."; }
const std::string MessageCodes::errorCannotSendToChan(const std::string& nick, const std::string& chan) { return _svr_prefix + " 404 " + nick + " " + chan + " :Cannot send to channel."; }
const std::string MessageCodes::errorNoOrigin(const std::string& nick) { return _svr_prefix + " 409 " + nick + " :No origin specified."; }
const std::string MessageCodes::errorNoRecipient(const std::string& nick) { return _svr_prefix + " 411 " + nick + " :No recipient given."; }
const std::string MessageCodes::errorNoTextToSend(const std::string& nick) { return _svr_prefix + " 412 " + nick + " :No text to send."; }
const std::string MessageCodes::errorUnknownCommand(const std::string& nick, const std::string& cmd) { return _svr_prefix + " 421 " + nick + " " + cmd + " :Unknown command"; }
const std::string MessageCodes::errorNonicknameGiven(const std::string& nick) { return _svr_prefix + " 431 " + nick + " :No nickname given."; }
const std::string MessageCodes::errorErroneusNickname(const std::string& nick, const std::string& wrong) { return _svr_prefix + " 432 " + nick + " " + wrong + " :Nickname is invalid."; }
const std::string MessageCodes::errorNicknameInUse(const std::string& nick, const std::string& wrong) { return _svr_prefix + " 433 " + nick + " " + wrong + " :Nickname is already in use."; }
const std::string MessageCodes::errorUserNotInChannel(const std::string& nick, const std::string& wrong, const std::string& chan) { return _svr_prefix + " 441 " + nick + " " + wrong + " " + chan + " :User not in this channel."; }
const std::string MessageCodes::errorNotOnChannel(const std::string& nick, const std::string& chan) { return _svr_prefix + " 442 " + nick + " " + chan + " :You're not in this channel."; }
const std::string MessageCodes::errorNotRegistered(const std::string& nick, const std::string& cmd) { return _svr_prefix + " 451 " + nick + " " + cmd + " :You must register."; }
const std::string MessageCodes::errorNeedMoreParams(const std::string& nick, const std::string& cmd) { return _svr_prefix + " 461 " + nick + " " + cmd + " :Need more parameters"; }
const std::string MessageCodes::errorAlreadyRegistered(const std::string& nick) { return _svr_prefix + " 462 " + nick + " :You are already registered."; }
const std::string MessageCodes::errorPasswdMismatch(const std::string& nick) { return _svr_prefix + " 464 " + nick + " :Incorrect password, access denied."; }
const std::string MessageCodes::errorChannelIsFull(const std::string& nick, const std::string& chan) { return _svr_prefix + " 471 " + nick + " " + chan + " :Cannot join channel (+l)."; }
const std::string MessageCodes::errorUnknownMode(const std::string& nick, const std::string& mode) { return _svr_prefix + " 472 " + nick + " " + mode + " :Unknown mode."; }
const std::string MessageCodes::errorInviteOnlyChan(const std::string& nick, const std::string& chan) { return _svr_prefix + " 473 " + nick + " " + chan + " :Cannot join channel (+i)."; }
const std::string MessageCodes::errorBadChannelKey(const std::string& nick, const std::string& chan) { return _svr_prefix + " 475 " + nick + " " + chan + " :Cannot join channel (+k)."; }
const std::string MessageCodes::errorNoPrivileges(const std::string& nick) { return _svr_prefix + " 481 " + nick + " :You are not server Operator."; }
const std::string MessageCodes::errorChanOpPrivsNeeded(const std::string& nick, const std::string& chan) { return _svr_prefix + " 482 " + nick + " " + chan + " :You're not channel operator."; }
const std::string MessageCodes::errorNoOperHost(const std::string& nick) { return _svr_prefix + " 491 " + nick + " :Operator status refused."; }
const std::string MessageCodes::errorUModeUnknownFlag(const std::string& nick) { return _svr_prefix + " 501 " + nick + " :Unknown MODE flag."; }
const std::string MessageCodes::errorUsersDontMatch(const std::string& nick) { return _svr_prefix + " 502 " + nick + " :Cannot change mode for other users."; }

const std::string MessageCodes::msgChannelDeleted(const std::string& chan) { return "Channel " + chan + " has been deleted."; }


void MessageCodes::msgDev(const std::string& msg, std::string& val) { std::cout << _green << "-> " << msg << val << _no_color << std::endl; }
void MessageCodes::msgErr(const std::string& msg) { std::cerr << _red << "Error: " << msg << _no_color << std::endl; }
