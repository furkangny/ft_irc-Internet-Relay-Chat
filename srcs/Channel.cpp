#include "Channel.hpp"

Channel::Channel(Server *server, std::string name, User *user):
_server(server), _channelName(name), _modes(0), _maxUsers(0)
{
	_messageCodes.msg_log(_messageCodes.msgChannelCreated(user->getNickname(), _channelName));
	_topic.empty();
	_password.empty();
	addUser(user, OPERATOR);
}

Channel::Channel(Server *server, std::string name, User *user, std::string const &password):
_server(server), _channelName(name), _modes(0), _password(password), _maxUsers(0)
{
	_messageCodes.msg_log(_messageCodes.msgChannelCreated(user->getNickname(), _channelName));
	_topic.empty();
	setMode(KEY, PLUS);
	addUser(user, OPERATOR);
}

Channel::~Channel()
{
	for (std::list<User *>::iterator it = _invitedUsers.begin(); it != _invitedUsers.end(); it++)
		(*it)->removeInvitedChannel(this);
	_messageCodes.msg_log(_messageCodes.msgChannelDeleted(_channelName));
}

void	Channel::sendToChannel(User *user, std::string const &msg)
{
	for (std::list<User *>::iterator it = _operators.begin(); it != _operators.end(); it++)
	{
		if (user == NULL || (*it)->getNickname() != user->getNickname())
				(*it)->sendToClient(msg);
	}
	for (std::list<User *>::iterator it = _normalUsers.begin(); it != _normalUsers.end(); it++)
	{
		if (user == NULL || (*it)->getNickname() != user->getNickname())
				(*it)->sendToClient(msg);
	}
}

std::string	const	Channel::sendTopic(User *user) const
{
	if (_topic.empty())
		return (_messageCodes.rplNoTopic(user->getNickname(), _channelName));
	return (_messageCodes.rplTopic(user->getNickname(), _channelName, _topic));
}

void	Channel::addUser(User *user, userLevel lvl)
{
	if (lvl == INVITED)
	{
		if (isInvited(user->getNickname()))
			return ;
		_invitedUsers.push_back(user);
		user->addInvitedChannel(this);
	}
	else
	{
		if (isInvited(user->getNickname()))
			_invitedUsers.remove(user);
		if (lvl == NORMAL)
			_normalUsers.push_back(user);
		else if (lvl == OPERATOR)
			_operators.push_back(user);
		sendToChannel(NULL, _messageCodes.sendJoin(user->getFullname(), _channelName));
		user->addJoinedChannel(this);
		_messageCodes.msg_log(user->getNickname() + " joined the channel " + _channelName);
		welcomeUser(user);
	}
}

void	Channel::setUserLevel(User *user, userLevel lvl)
{
	if (lvl == NORMAL && !isNormal(user->getNickname()))
	{
		_operators.remove(user);
		_normalUsers.push_back(user);
	}
	else if (lvl == OPERATOR && !isOperator(user->getNickname()))
	{
		_normalUsers.remove(user);
		_operators.push_back(user);
	}
}

void	Channel::removeUser(User *user)
{
	if (isInvited(user->getNickname()))
		_invitedUsers.remove(user);
	else
	{
		if (isOperator(user->getNickname()))
			_operators.remove(user);
		else
			_normalUsers.remove(user);
		_messageCodes.msg_log(user->getNickname() + " has left the channel " + _channelName);

		if (isChannelEmpty())
			_server->deleteChannel(this);
	}

}

User	*Channel::findUserInChannel(std::string nickname)
{
	User *user = NULL;
	user = findUserFromList(_operators, nickname);
	if (user == NULL)
		user = findUserFromList(_normalUsers, nickname);
	return user;
}

void	Channel::displayUsers(User *user)
{
	
	std::string	msg;
	bool		first = true;

	for (std::list<User *>::iterator it = _operators.begin(); it != _operators.end(); it++)
	{
		if (first)
		{
			msg += "@";
			msg += (*it)->getNickname();
			first = false;
		}
		else
		{
			msg += " @";
			msg += (*it)->getNickname();
		}
	}
	first = true;
	for (std::list<User *>::iterator it = _normalUsers.begin(); it != _normalUsers.end(); it++)
	{
			msg += " ";
			msg += (*it)->getNickname();
	}
	if (!msg.empty())
	{
		user->sendToClient(_messageCodes.rplNameReply(user->getNickname(), _channelName, msg));
		user->sendToClient(_messageCodes.rplEndOfNames(user->getNickname(), _channelName));
	}
}

void	Channel::welcomeUser(User *user)
{
	if (!_topic.empty())
		user-> sendToClient(_messageCodes.rplTopic(user->getNickname(), _channelName, _topic));
	displayUsers(user);
}

bool	Channel::isPasswordCorrect(std::string const &pass) { return (pass == _password); }
void	Channel::setMode(channelModes mode, modeType type) { (type == PLUS ? _modes |= mode : _modes &= ~mode); }
bool	Channel::isMode(channelModes mode) const { return ((_modes & mode) != 0); }

std::string	Channel::listModes(bool isMember) const
{
	std::string	modes = "";

	if (isMode(INVITE_ONLY))
		modes += "i";
	if (isMode(KEY))
		modes += "k";
	if (isMode(LIMIT))
		modes += "l";
	if (isMode(TOPIC))
		modes += "t";
	if (isMember)
	{
		if (isMode(KEY))
			modes += " " + _password;
		if (isMode(LIMIT))
			modes += " " + _messageCodes.to_string(_maxUsers);
	}
	return (modes);
}
std::string const	&Channel::getChannelName() const { return (_channelName); }
std::string const	&Channel::getTopic() const { return (_topic); }
std::string const	&Channel::getPassword() const { return(_password); }
int					Channel::getMaxUsers() const { return(_maxUsers); }
bool	Channel::isInvited(std::string const &nickname) { return (findUserFromList(_invitedUsers, nickname) != NULL);  }
bool	Channel::isOperator(std::string const &nickname) { return (findUserFromList(_operators, nickname) != NULL); }
bool	Channel::isNormal(std::string const &nickname) { return (findUserFromList(_normalUsers, nickname) != NULL); }
int		Channel::getTotalUsers() const { return (_operators.size() + _normalUsers.size()); }
void	Channel::setPassword(std::string const &password) { _password = password; }
void	Channel::setTopic(std::string const &topic) { _topic = topic; }
void	Channel::setMaxUsers(int max) { _maxUsers = max; }

User	*Channel::findUserFromList(std::list<User *> &role, std::string nickname)
{
	user_iterator it;
	for (it = role.begin(); it != role.end(); it++){
		if((*it)->getNickname() == nickname)
			return (*it);
	}
	return NULL;
}

bool	Channel::isChannelEmpty()
{
	if (_operators.size() + _normalUsers.size() == 0)
		return (true);

	User	*user;
	if (_operators.size() == 0)
	{
		user = _normalUsers.front();
		setUserLevel(user, OPERATOR);
	}
	return (false);
}