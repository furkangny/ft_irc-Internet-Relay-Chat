#include "User.hpp"

User::User(Server *server, int clientSocket, std::string const &clientHostname):
	_server(server), _socket_fd(clientSocket), _hostname(clientHostname), _op(false)
{
	_status = CREATED;
	_nickname = "*";
	_username = "*";
	_realname = "*";
	_leavingMsg = "*";
	_joinedChannels.clear();
	_invitedChannels.clear();
}

User::~User()
{
	for (channel_iterator it = _invitedChannels.begin(); it != _invitedChannels.end(); it++)
		(*it)->removeUser(this);
	_invitedChannels.clear();
}
bool	User::isEmpty(std::string const &str) const { return str == "*"; }

void	User::sendToClient(std::string const &msg)
{
	std::string	fullMsg = msg + "\r\n";
	
	if (send(_socket_fd, fullMsg.c_str(), fullMsg.length(), 0) == _messageCodes.getError())
		_messageCodes.msgErr(strerror(errno));
}

void	User::welcome()
{
	sendToClient(_messageCodes.rplWelcome(_nickname, getFullname()));
	sendToClient(_messageCodes.rplYourHost(_nickname));
	sendToClient(_messageCodes.rplCreated(_nickname));
	sendToClient(_messageCodes.rplMyInfo(_nickname));
}

void	User::addJoinedChannel(Channel *channel)	{ _joinedChannels.push_back(channel); }
void	User::addInvitedChannel(Channel *channel)	{ _invitedChannels.push_back(channel); }

void	User::removeJoinedChannel(Channel *channel)
{
	channel_iterator	it = std::find(_joinedChannels.begin(), _joinedChannels.end(), channel);
	if (it != _joinedChannels.end())
		_joinedChannels.erase(it);
}

void	User::removeInvitedChannel(Channel *channel)
{
	channel_iterator	it = std::find(_invitedChannels.begin(), _invitedChannels.end(), channel);
	if (it != _invitedChannels.end())
		_invitedChannels.erase(it);
}

void	User::leaveChannel(Channel *channel, User *origin, std::string const &reason)
{
	if (reason == "PART")
	{
		if (isEmpty(_leavingMsg))
			channel->sendToChannel(NULL, _messageCodes.sendPart(getFullname(), channel->getChannelName()));
		else
			channel->sendToChannel(NULL, _messageCodes.sendPartMsg(getFullname(), channel->getChannelName(), _leavingMsg));
	}
	else if (reason == "KICK")
	{
		if (isEmpty(_leavingMsg))
		{
			channel->sendToChannel(NULL, _messageCodes.sendKick(origin->getFullname(), channel->getChannelName(), _nickname, origin->getNickname()));
			_messageCodes.msg_log(_nickname + " has been kicked from " + channel->getChannelName());
		}
		else
		{
			channel->sendToChannel(NULL, _messageCodes.sendKick(origin->getFullname(), channel->getChannelName(), _nickname, _leavingMsg));
			_messageCodes.msg_log(_nickname + " has been kicked from " + channel->getChannelName() + " :" + _leavingMsg);
		}
	}
	removeJoinedChannel(channel);
	channel->removeUser(this);
	_leavingMsg = "*";
}

void	User::leaveAllChannels(std::string const &reason)
{
	channel_iterator it = _joinedChannels.begin();
	while (it != _joinedChannels.end())
	{
		Channel	*channel = *it;
		if (isEmpty(_leavingMsg))
		{
			if (reason == "PART")
				channel->sendToChannel(this, _messageCodes.sendPart(getFullname(), channel->getChannelName()));
			else
				channel->sendToChannel(this, _messageCodes.sendQuit(getFullname(), _nickname));
		}
		else
		{
			if (reason == "PART")
				channel->sendToChannel(this, _messageCodes.sendPartMsg(getFullname(), channel->getChannelName(), _leavingMsg));
			else
				channel->sendToChannel(this, _messageCodes.sendQuitMsg(getFullname(), _leavingMsg));
		}
		it = _joinedChannels.erase(it);
		channel->removeUser(this);
	}
	_leavingMsg = "*";
	_joinedChannels.clear();
}

bool				User::isServerOp() const	{ return _op; }
int 				User::getSocketFd() const	{ return _socket_fd; }
clientStatus		User::getStatus() const		{ return _status; }
std::string const	&User::getUsername() const	{ return _username; }
std::string	const	&User::getNickname() const	{ return _nickname; }
std::string const	&User::getRealname() const	{ return _realname; }
std::string const	&User::getHostname() const	{ return _hostname; }
std::string			User::getFullname() const
{
	std::string fullMsg = _nickname;
	fullMsg += isEmpty(_username) ? "" : "!~" +_username;
	fullMsg += isEmpty(_hostname) ? "" : "@" +_hostname;

	return (fullMsg);
}
void	User::setStatus(clientStatus status)			{ _status = status; }
void	User::setUsername(std::string const &username)	{ _username = username; }
void	User::setNickname(std::string const &nickname)	{ _nickname = nickname; }
void	User::setRealname(std::string const &realname)	{ _realname = realname; }
void	User::setHostname(std::string const &hostname)	{ _hostname = hostname; }
void	User::setLeavingMessage(std::string const &msg)	{ _leavingMsg = msg; }
void	User::setServerOP(bool val)						{ _op = val; }