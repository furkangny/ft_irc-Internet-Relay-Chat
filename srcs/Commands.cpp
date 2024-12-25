#include "Commands.hpp"

void	initCommands(Server *server, std::map<std::string, Command *> &commands)
{
	commands["QUIT"] = new Quit(server);
	commands["PASS"] = new Pass(server);
	commands["NICK"] = new Nick(server);
	commands["USER"] = new UserCMD(server);
	commands["PING"] = new Ping(server);
	commands["JOIN"] = new Join(server);
	commands["PART"] = new Part(server);
	commands["KICK"] = new Kick(server);
	commands["INVITE"] = new Invite(server);
	commands["PRIVMSG"] = new Privmsg(server);
	commands["TOPIC"] = new Topic(server);
	commands["MODE"] = new Mode(server);
	commands["OPER"] = new Oper(server);
	commands["POWEROFF"] = new Poweroff(server);
}

void	execute(Server	*server, User *user, s_msg &msg)
{
	MessageCodes	_messageCodes;

	if (msg.cmd == "QUIT")
		(*server->getCommands()["QUIT"]).execute(user, msg);
	else if (msg.cmd == "PASS")
		(*server->getCommands()["PASS"]).execute(user, msg);
	else if (msg.cmd == "NICK")
		(*server->getCommands()["NICK"]).execute(user, msg);
	else if (msg.cmd == "USER")
		(*server->getCommands()["USER"]).execute(user, msg);
	else if (msg.cmd == "PING")
		(*server->getCommands()["PING"]).execute(user, msg);
	else if (msg.cmd == "JOIN")
		(*server->getCommands()["JOIN"]).execute(user, msg);
	else if (msg.cmd == "PART")
		(*server->getCommands()["PART"]).execute(user, msg);
	else if (msg.cmd == "KICK")
		(*server->getCommands()["KICK"]).execute(user, msg);
	else if (msg.cmd == "INVITE")
		(*server->getCommands()["INVITE"]).execute(user, msg);
	else if (msg.cmd == "PRIVMSG")
		(*server->getCommands()["PRIVMSG"]).execute(user, msg);
	else if (msg.cmd == "TOPIC")
		(*server->getCommands()["TOPIC"]).execute(user, msg);
	else if (msg.cmd == "MODE")
		(*server->getCommands()["MODE"]).execute(user, msg);
	else if (msg.cmd == "OPER")
		(*server->getCommands()["OPER"]).execute(user, msg);
	else if (msg.cmd == "POWEROFF")
		(*server->getCommands()["POWEROFF"]).execute(user, msg);
	else if (msg.cmd == "CAP")
		;
	else
		user->sendToClient(_messageCodes.errorUnknownCommand(user->getNickname(), msg.cmd));
}

void	deleteCommands(std::map<std::string, Command *> &commands)
{
	std::map<std::string, Command *>::iterator it = commands.begin();
	std::map<std::string, Command *>::iterator end = commands.end();

	while (it != end)
	{
		delete it->second;
		it++;
	}
}

Command::Command(Server	*server): _server(server) {   }
Command::~Command() {  }

Quit::Quit(Server *server) : Command(server) {  }
Quit::~Quit() {  }

void	Quit::execute(User *user, s_msg &msg)
{
	if (!msg.trailing.empty())
		user->setLeavingMessage(msg.trailing);
	user->sendToClient(_messageCodes.msgClientQuit(user->getNickname()));
	_server->disconnectClient(user);
}

Pass::Pass(Server *server): Command(server) {  }
Pass::~Pass() {  }

void Pass::execute(User *user, s_msg &msg)
{
    std::string received_password;

    if (msg.args.empty() && !msg.trailing.empty()) {
        received_password = msg.trailing;
    }
    else if (!msg.args.empty()) {
        received_password = msg.args[0];
    }
    else {
        std::string errorMsg = _messageCodes.errorNeedMoreParams(user->getNickname(), "PASS");
        user->sendToClient(errorMsg);
        return;
    }
    received_password.erase(received_password.find_last_not_of("\r\n") + 1);

    if (_server->getPassword() != received_password) {
		std::string errorMsg = _messageCodes.errorPasswdMismatch(user->getNickname());
		user->sendToClient(errorMsg);
        return;
    }
    user->setStatus(PASSWORDACCEPTED);
}

static bool	isNickValid(std::string const &nick)
{
	if (nick.length() > 9)
		return (false);
	if (!isalpha(nick[0]) && nick[0] != '|' && nick[0] != '^' && nick[0] != '~')
		return (false);
	for (std::string::const_iterator it = nick.begin(); it != nick.end(); ++it)
	{
		if (!isalnum(*it) && *it != '-' && *it != '[' && *it != ']' && *it != '\\' && *it != '{' && *it != '}' && *it != '|' && *it != '^' && *it != '_')
			return (false);
	}
	return (true);
}

Nick::Nick(Server* server) : Command(server) {  }
Nick::~Nick() {  }

void	Nick::execute(User *user, s_msg &msg)
{
	if (msg.args.empty() || msg.args[0].empty())
		user->sendToClient(_messageCodes.errorNonicknameGiven(user->getNickname()));
	else
	{
		if (!isNickValid(msg.args[0]))
			user->sendToClient(_messageCodes.errorErroneusNickname(user->getNickname(), msg.args[0]));
		else if (_server->getUserWithNickname(msg.args[0]))
			user->sendToClient(_messageCodes.errorNicknameInUse(user->getNickname(), msg.args[0]));
		else
		{
			if (user->getStatus() == PASSWORDACCEPTED)
			{
				user->setNickname(msg.args[0]);
				user->setStatus(NICKNAMEISOK);
			}
			else if (user->getStatus() == USERNAMEISOK)
			{
				user->setNickname(msg.args[0]);
				user->setStatus(REGISTERED);
				user->welcome();
			}
			else if (user->getStatus() == NICKNAMEISOK || user->getStatus() == REGISTERED) 
			{
				if (user->getStatus() == REGISTERED)
					user->sendToClient(_messageCodes.sendNick(user->getFullname(), msg.args[0]));
				user->setNickname(msg.args[0]);
			}
			_messageCodes.msg_log(_messageCodes.msgClientNick(user->getSocketFd(), user->getNickname()));
		}
	}
}

static bool	isUsernameValid(std::string const &username)
{
	if (username.length() > 9)
		return (false);	
	if (!isalpha(username[0]))
		return (false);

	for (std::string::const_iterator it = username.begin(); it != username.end(); ++it)
	{
		if (!isalnum(*it) && *it != '-' && *it != '[' && *it != ']' && *it != '\\' && *it != '{' && *it != '}' && *it != '|' && *it != '^' && *it != '_')
			return (false);
	}
	return (true);
}

static bool	isRealnameValid(std::string const &realname)
{
	return !realname.empty();
}

UserCMD::UserCMD(Server *server) : Command(server) {  }
UserCMD::~UserCMD() {  }

void	UserCMD::execute(User *user, s_msg &msg)
{
	if (!(msg.args.size() == 3 && !msg.trailing.empty()))
		user->sendToClient(_messageCodes.errorNeedMoreParams(user->getNickname(), "USER"));
	else if (!isUsernameValid(msg.args[0]))
		user->sendToClient(_messageCodes.errInvalidUsername(msg.args[0]));
	else if (!isRealnameValid(msg.trailing))
		user->sendToClient(_messageCodes.errInvalidRealname(msg.trailing));
	else if (user->getStatus() == REGISTERED)
		user->sendToClient(_messageCodes.errorAlreadyRegistered(user->getNickname()));
	else
	{
		if (user->getStatus() == PASSWORDACCEPTED)
		{
			user->setUsername(msg.args[0]);
			user->setRealname(msg.trailing);
			user->setStatus(USERNAMEISOK);
		}
		else if (user->getStatus() == NICKNAMEISOK)
		{
			user->setUsername(msg.args[0]);
			user->setRealname(msg.trailing);
			user->setStatus(REGISTERED);
			user->welcome();
		}
		else if (user->getStatus() == USERNAMEISOK)
		{
			user->setUsername(msg.args[0]);
			user->setRealname(msg.trailing);
		}
		_messageCodes.msg_log(_messageCodes.msgClientUser(user->getSocketFd(), user->getUsername(), user->getRealname()));
	}
}

Ping::Ping(Server *server): Command(server) {   }
Ping::~Ping() {  }

void	Ping::execute(User *user, s_msg &msg)
{
	if (user->getStatus() != REGISTERED)
		user->sendToClient(_messageCodes.errorNotRegistered(user->getNickname(), "PING"));
	else if (msg.args.empty())
		user->sendToClient(_messageCodes.errorNoOrigin(user->getNickname()));
	else
		user->sendToClient(_messageCodes.sendPong(user->getNickname()));
}

static bool	isChanNameValid(std::string const &name)
{
	if (name.length() > 64)
		return (false);
	if (name[0] != '#' && name[0] != '&')
		return (false);
	for (std::string::const_iterator it = name.begin() + 1; it != name.end(); ++it)
	{
		if (!isalnum(*it) && *it != '-' && *it != '.'&& *it != '_')
			return (false);
	}
	return (true);
}

static void	parseJoin(std::vector<std::string> &chan_list, std::vector<std::string> &key_list, std::vector<std::string> const &args)
{
	std::istringstream iss_chan(args[0]);
	std::string chan;
	while (std::getline(iss_chan, chan, ','))
		chan_list.push_back(chan);

	if (args.size() == 1)
		return ;

	std::istringstream iss_key(args[1]);
	std::string key;
	while (std::getline(iss_key, key, ','))
		key_list.push_back(key);
}

Join::Join(Server *server): Command(server) {  }
Join::~Join() {  }

void	Join::execute(User *user, s_msg &msg)
{
	if (user->getStatus() != REGISTERED)
		user->sendToClient(_messageCodes.errorNotRegistered(user->getNickname(), "JOIN"));
	else if (msg.args.empty())
		user->sendToClient(_messageCodes.errorNeedMoreParams(user->getNickname(), "JOIN"));
	else if (msg.args[0] == "0")
		user->leaveAllChannels("PART");
	else
	{
		std::vector<std::string>	chan_list;
		std::vector<std::string>	key_list;

		parseJoin(chan_list, key_list, msg.args);

		for (std::vector<std::string>::iterator it = chan_list.begin(); it != chan_list.end(); it++)
		{
			if (!isChanNameValid(*it))
				user->sendToClient(_messageCodes.errBadChannelName(user->getNickname(), *it));
			else
			{
				Channel *channel = _server->findChannel(*it);
				if (channel)
				{
					if (channel->findUserInChannel(user->getNickname()))
						(void)user;
					else if (channel->getMaxUsers() > 0 && channel->getTotalUsers() >= channel->getMaxUsers())
						user->sendToClient(_messageCodes.errorChannelIsFull(user->getNickname(), channel->getChannelName()));
					else if (channel->isMode(INVITE_ONLY) && !channel->isInvited(user->getNickname()))
						user->sendToClient(_messageCodes.errorInviteOnlyChan(user->getNickname(), channel->getChannelName()));
					else
					{
						if (channel->isMode(KEY))
						{
							if (key_list.empty())
								user->sendToClient(_messageCodes.errorBadChannelKey(user->getNickname(), channel->getChannelName()));
							else
							{
								if (!channel->isPasswordCorrect(key_list.front()))
									user->sendToClient(_messageCodes.errorBadChannelKey(user->getNickname(), channel->getChannelName()));
								else
									channel->addUser(user, NORMAL);
								key_list.erase(key_list.begin());
							}
						}
						else
						{
							if (!key_list.empty())
								key_list.erase(key_list.begin());
							channel->addUser(user, NORMAL);
						}
					}
				}
				else
				{
					if (!key_list.empty())
					{
						_server->newChannel(*it, user, key_list.front());
						key_list.erase(key_list.begin());
					}
					else
					{
						_server->newChannel(*it, user);
					}
				}
			}
		}
	}
}

static void	parsePart(std::vector<std::string> &chan_list, std::string const &args)
{
	std::istringstream iss_chan(args);
	std::string chan;
	while (std::getline(iss_chan, chan, ','))
		chan_list.push_back(chan);
}

Part::Part(Server *server) : Command(server) {  }
Part::~Part() {  }

void	Part::execute(User *user, s_msg &msg)
{
	if (user->getStatus() != REGISTERED)
		user->sendToClient(_messageCodes.errorNotRegistered(user->getNickname(), "PART"));
	else if (msg.args.empty())
			user->sendToClient(_messageCodes.errorNeedMoreParams(user->getNickname(), "PART"));
	else
	{
		std::vector<std::string>	chan_list;

		parsePart(chan_list, msg.args[0]);
		for (std::vector<std::string>::iterator it = chan_list.begin(); it != chan_list.end(); it++)
		{
			Channel	*channel = _server->findChannel(*it);

			if (!channel)
				user->sendToClient(_messageCodes.errorNoSuchChannel(user->getNickname(), *it));
			else if (!channel->findUserInChannel(user->getNickname()))
				user->sendToClient(_messageCodes.errorNotOnChannel(user->getNickname(), channel->getChannelName()));
			else
			{
				if (!msg.trailing.empty())
					user->setLeavingMessage(msg.trailing);
				user->leaveChannel(channel, NULL, "PART");
			}
		}
	}
}

static void	parseKick(std::vector<std::string> &chan_list, std::vector<std::string> &user_list, std::vector<std::string> const &args)
{
	std::istringstream iss_chan(args[0]);
	std::string chan;
	while (std::getline(iss_chan, chan, ','))
		chan_list.push_back(chan);

	std::istringstream iss_user(args[1]);
	std::string user;
	while (std::getline(iss_user, user, ','))
		user_list.push_back(user);
}

Kick::Kick(Server* server) : Command(server) {  }
Kick::~Kick() {  }

void	Kick::execute(User *user, s_msg &msg)
{
	if (user->getStatus() != REGISTERED)
		user->sendToClient(_messageCodes.errorNotRegistered(user->getNickname(), "KICK"));
	else if (msg.args.empty() || msg.args.size() != 2)
		user->sendToClient(_messageCodes.errorNeedMoreParams(user->getNickname(), "KICK"));
	else
	{
		std::vector<std::string>	chan_list;
		std::vector<std::string>	user_list;

		parseKick(chan_list, user_list, msg.args);
		for (std::vector<std::string>::iterator it = chan_list.begin(); it != chan_list.end(); it++)
		{
			Channel	*channel = _server->findChannel(*it);
			if (!channel)
				user->sendToClient(_messageCodes.errorNoSuchChannel(user->getNickname(), msg.args[0]));
			else
			{
				if (!channel->findUserInChannel(user->getNickname())) 
					user->sendToClient(_messageCodes.errorNotOnChannel(user->getNickname(), channel->getChannelName()));
				else if (!channel->isOperator(user->getNickname()))
						user->sendToClient(_messageCodes.errorChanOpPrivsNeeded(user->getNickname(), channel->getChannelName()));
				else
				{
					for (std::vector<std::string>::iterator it2 = user_list.begin(); it2 != user_list.end(); it2++)
					{
						User	*target = _server->getUserWithNickname(*it2);
						if (!target)
							user->sendToClient(_messageCodes.errorNoSuchNick(user->getNickname(), *it2));
						else if (!channel->findUserInChannel(target->getNickname()))
							user->sendToClient(_messageCodes.errorUserNotInChannel(user->getNickname(), target->getNickname(), channel->getChannelName()));
						else
						{
							target->setLeavingMessage(msg.trailing);
							target->leaveChannel(channel, user, "KICK");
						}
					}
				}
			}
		}
	}
}
Invite::Invite(Server *server) : Command(server) {  }
Invite::~Invite() {  }

void	Invite::execute(User *user, s_msg &msg)
{
	if (user->getStatus() != REGISTERED)
		user->sendToClient(_messageCodes.errorNotRegistered(user->getNickname(), "INVITE"));
	else if (msg.args.empty())
		user->sendToClient(_messageCodes.errorNeedMoreParams(user->getNickname(), "INVITE"));
	else if (msg.args.size() != 2)
		user->sendToClient(_messageCodes.errSyntax(user->getNickname(), "INVITE"));
	else
	{
		User	*target = _server->getUserWithNickname(msg.args[0]);
		Channel	*channel = _server->findChannel(msg.args[1]);

		if (target == NULL)
			user->sendToClient(_messageCodes.errorNoSuchNick(user->getNickname(), msg.args[0]));
		else if (channel == NULL || channel->isInvited(user->getNickname()))
			return ;
		else if (channel->findUserInChannel(target->getNickname()) == NULL)
		{
			channel->addUser(target, INVITED);
			target->sendToClient(_messageCodes.sendInvite(user->getFullname(), user->getNickname(), channel->getChannelName()));
			user->sendToClient(_messageCodes.rplInviting(user->getNickname(), target->getNickname(), channel->getChannelName()));
		}
	}
}
static void	parsePrivmsg(std::vector<std::string> &recipient_list, std::string const &args)
{
	if (args.find(',') == std::string::npos)
        recipient_list.push_back(args);
	else
	{
		std::istringstream	iss(args);
		std::string			recipient;
		while (std::getline(iss, recipient, ','))
			recipient_list.push_back(recipient);
	}
}

Privmsg::Privmsg(Server *server): Command(server) {  }
Privmsg::~Privmsg() {  }

void	Privmsg::execute(User *user, s_msg &msg)
{
	if (user->getStatus() != REGISTERED)
		user->sendToClient(_messageCodes.errorNotRegistered(user->getNickname(), "PRIVMSG"));
	else if (msg.args.empty())
		user->sendToClient(_messageCodes.errorNoRecipient(user->getNickname()));
	else if (msg.trailing.empty())
		user->sendToClient(_messageCodes.errorNoTextToSend(user->getNickname()));
	else if (msg.args.size() != 1)
		user->sendToClient(_messageCodes.errSyntax(user->getNickname(), "PRIVMSG"));
	else
	{
		std::vector<std::string>	recipient_list;

		parsePrivmsg(recipient_list, msg.args[0]);
		if (recipient_list.empty())
			user->sendToClient(_messageCodes.errorNoRecipient(user->getNickname()));
		else
		{
			for (std::vector<std::string>::const_iterator it = recipient_list.begin(); it != recipient_list.end(); ++it)
			{
				std::string const	&recipient = *it;
				if (recipient[0] == '#' || recipient[0] == '&')
				{
					Channel	*channel = _server->findChannel(recipient);
					if (channel == NULL)
						user->sendToClient(_messageCodes.errorNoSuchChannel(user->getNickname(), recipient));
					else if (!channel->findUserInChannel(user->getNickname()))
						user->sendToClient(_messageCodes.errorCannotSendToChan(user->getNickname(), recipient));
					else
						channel->sendToChannel(user, _messageCodes.sendPm(user->getFullname(), channel->getChannelName(), msg.trailing));
				}
				else
				{
					User	*target = _server->getUserWithNickname(recipient);
					if (target == NULL)
						user->setNickname(_messageCodes.errorNoSuchNick(user->getNickname(), recipient));
					else
						target->sendToClient(_messageCodes.sendPm(user->getFullname(), target->getNickname(), msg.trailing));
				}

			}
		}
	}
}

Topic::Topic(Server *server) : Command(server) {  }
Topic::~Topic() {  }

void	Topic::execute(User *user, s_msg &msg)
{
	if (user->getStatus() != REGISTERED)
		user->sendToClient(_messageCodes.errorNotRegistered(user->getNickname(), "TOPIC"));
	else if (msg.args.empty() && msg.trailing.empty())
		user->sendToClient(_messageCodes.errorNeedMoreParams(user->getNickname(), "TOPIC"));
	else if (msg.args.empty() && !msg.trailing.empty())
		user->sendToClient(_messageCodes.errorNoSuchChannel(user->getNickname(), ":" + msg.trailing));
	else
	{
		Channel	*channel = _server->findChannel(msg.args[0]);
		if (!channel)
			user->sendToClient(_messageCodes.errorNoSuchChannel(user->getNickname(), msg.args[0]));
		else if (msg.args.size() == 1 && msg.trailing_sign == false)
			user->sendToClient(channel->sendTopic(user));
		else if (!channel->findUserInChannel(user->getNickname()) )
			user->sendToClient(_messageCodes.errorNotOnChannel(user->getNickname(), channel->getChannelName()));
		else
		{
			if (channel->isMode(TOPIC) && !channel->isOperator(user->getNickname()))
				user->sendToClient(_messageCodes.errorChanOpPrivsNeeded(user->getNickname(), channel->getChannelName()));
			else
			{
				if (msg.args.size() == 1 && msg.trailing.empty() && msg.trailing_sign == true)
				{
					channel->setTopic("");
					channel->sendToChannel(NULL, _messageCodes.sendTopic(user->getFullname(), channel->getChannelName(), ""));
				}
				else if (msg.args.size() > 1)
				{
					channel->setTopic(msg.args[1]);
					channel->sendToChannel(NULL, _messageCodes.sendTopic(user->getFullname(), channel->getChannelName(), channel->getTopic()));
				}
				else
				{
					channel->setTopic(msg.trailing);
					channel->sendToChannel(NULL, _messageCodes.sendTopic(user->getFullname(), channel->getChannelName(), channel->getTopic()));
				}
			}
		}
	}
}

static char		type_to_c(modeType type) { return (type == PLUS ? '+' : '-'); }

static modeVal	c_to_val(char c)
{
	switch(c)
	{
		case 'i': return (I_MODE);
		case 'k': return (K_MODE);
		case 'l': return (L_MODE);
		case 'o': return (O_MODE);
		case 't': return (T_MODE);
		default: return (BAD_MODE);
	}
}

Mode::Mode(Server *server): Command(server) { _last = PLUS; }
Mode::~Mode() {  }

static std::string	parseMode(std::queue<std::pair<char, modePair> > &mods, std::queue<std::string> &params, std::vector<std::string> &args)
{
	modeType	last = PLUS;
	std::string	target = args[0];
	if (args.size() == 1)
		return target;

	for (std::string::iterator it = args[1].begin(); it != args[1].end(); ++it)
	{
		char	c = *it;
		if (c == '+')
			last = PLUS;
		else if (c == '-')
			last = MINUS;
		else
		{
			modePair	mp;
			mp.first = last;
			mp.second = c_to_val(c);
			std::pair<char, modePair>	result;
			result.first = c;
			result.second = mp;
			mods.push(result);
		}
	}

    for (size_t i = 2; i < args.size(); ++i) {
        params.push(args[i]);
    }
	
	return (target);
}

void	Mode::execute(User *user, s_msg &msg)
{
	if (user->getStatus() != REGISTERED)
		user->sendToClient(_messageCodes.errorNotRegistered(user->getNickname(), "MODE"));
	else if (msg.args.empty())
		user->sendToClient(_messageCodes.errorNeedMoreParams(user->getNickname(), "MODE"));
	else
	{
		std::queue<std::pair<char, modePair> >		mods;
		std::queue<std::string>						params;
		std::string									target = parseMode(mods, params, msg.args);
		Channel										*channel = _server->findChannel(target);

		_modsToSend.clear();
		_paramsToSend.clear();
		if (target != user->getNickname() && _server->getUserWithNickname(target) != NULL)
			user->sendToClient(_messageCodes.errorUsersDontMatch(user->getNickname()));
		else if (_server->getUserWithNickname(target) == NULL && !channel)
			user->sendToClient(_messageCodes.errorNoSuchChannel(user->getNickname(), target));
		else if (target == user->getNickname())
		{
			if (mods.empty())
				user->sendToClient(_messageCodes.rplUModeIs(user->getNickname(), (user->isServerOp() ? "+o" : "+")));
			else
			{
				bool	gotwrong = false;

				while (!mods.empty())
				{
					std::pair<char, modePair>	it = mods.front();
					modePair	mp = it.second;
					if (mp.second == O_MODE)
					{
							if (mp.first == MINUS)
							{
								user->setServerOP(false);
								user->sendToClient(_messageCodes.sendModeUser(user->getFullname(), user->getNickname(), "-o"));
							}

					}
					else
						gotwrong = true;
					mods.pop();
				}
				if (!params.empty())
					gotwrong = true;
				if (gotwrong)
					user->sendToClient(_messageCodes.errorUModeUnknownFlag(user->getNickname()));
			}
		}
		else
		{
			if (mods.empty())
			{
				if (channel->findUserInChannel(user->getNickname()) != NULL)
					user->sendToClient(_messageCodes.rplChannelModeIs(user->getNickname(), channel->getChannelName(), channel->listModes(true)));
				else
					user->sendToClient(_messageCodes.rplChannelModeIs(user->getNickname(), channel->getChannelName(), channel->listModes(false)));
			}
			else if (!channel->isOperator(user->getNickname()))
				user->sendToClient(_messageCodes.errorChanOpPrivsNeeded(user->getNickname(), channel->getChannelName()));
			else
			{
				bool	happened[5] = {false};

				while (!mods.empty())
				{
					std::pair<char, modePair>	it = mods.front();
					modePair	mp = it.second;
					if (mp.second == BAD_MODE)
						user->sendToClient(_messageCodes.errorUnknownMode(user->getNickname(), _messageCodes.to_string(it.first)));
					else if (mp.second == I_MODE && !happened[I_MODE])
					{
						channel->setMode(INVITE_ONLY, mp.first);
						happened[I_MODE] = true;
						if (_last != mp.first || _modsToSend.empty())
						{
							_last = mp.first;
							_modsToSend += type_to_c(_last);
						}
						_modsToSend += 'i';
					}
					else if (mp.second == K_MODE && !happened[K_MODE] && !params.empty())
					{
						if (mp.first == PLUS)
						{
							channel->setPassword(params.front());
							if (!_paramsToSend.empty())
								_paramsToSend += " ";
							_paramsToSend += params.front();
						}
						else
						{
							if (!_paramsToSend.empty())
								_paramsToSend += " ";
							if (channel->isMode(KEY))
								_paramsToSend += channel->getPassword();
							else
								_paramsToSend += params.front();
							channel->setPassword("");
						}
						channel->setMode(KEY, mp.first);
						happened[K_MODE] = true;

						if (_last != mp.first || _modsToSend.empty())
						{
							_last = mp.first;
							_modsToSend += type_to_c(_last);
						}
						_modsToSend += 'k';
						params.pop();
					}
					else if (mp.second == L_MODE && !happened[L_MODE])
					{
						if (mp.first == PLUS && params.empty())
							user->sendToClient(_messageCodes.errorNeedMoreParams(user->getNickname(), "MODE +l"));
						else if (mp.first == MINUS && params.empty())
						{
							channel->setMaxUsers(0);
							channel->setMode(LIMIT, MINUS);
							if (_last != mp.first || _modsToSend.empty())
							{
								_last = mp.first;
								_modsToSend += type_to_c(_last);
							}
							_modsToSend += 'l';
						}
						else
						{
							std::string	p = params.front();
							int			max = atoi(p.c_str());

							if (mp.first == MINUS)
							{
								channel->setMaxUsers(0);
								channel->setMode(LIMIT, MINUS);
							}
							else
							{
								if (max > 0)
								{
									channel->setMaxUsers(max);
									channel->setMode(LIMIT, PLUS);
									if (!_paramsToSend.empty())
										_paramsToSend += " ";
									_paramsToSend += params.front();
								}
							}
							if (max > 0 || mp.first == MINUS)
							{
								if (_last != mp.first || _modsToSend.empty())
								{
									_last = mp.first;
									_modsToSend += type_to_c(_last);
								}
								_modsToSend += 'l';
							}
							params.pop();
						}
					}
					else if (mp.second == O_MODE && !happened[O_MODE] && !params.empty())
					{
						if (_server->getUserWithNickname(params.front()) == NULL)
						{
							user->sendToClient(_messageCodes.errorNoSuchNick(user->getNickname(), params.front()));
							user->sendToClient(_messageCodes.errorUserNotInChannel(user->getNickname(), params.front(), channel->getChannelName()));
						}
						else if (channel->findUserInChannel(params.front()) == NULL)
							user->sendToClient(_messageCodes.errorUserNotInChannel(user->getNickname(), params.front(), channel->getChannelName()));
						else
						{
							if (mp.first == PLUS)
								channel->setUserLevel(channel->findUserInChannel(params.front()), OPERATOR);
							else
								channel->setUserLevel(channel->findUserInChannel(params.front()), NORMAL);
							happened[O_MODE] = true;
							if (_last != mp.first || _modsToSend.empty())
							{
								_last = mp.first;
								_modsToSend += type_to_c(_last);
							}
							_modsToSend += 'o';
							if (!_paramsToSend.empty())
								_paramsToSend += " ";
							_paramsToSend += params.front();
							params.pop();
						}
					}
					else if (mp.second == T_MODE && !happened[T_MODE])
					{
						channel->setMode(TOPIC, mp.first);
						happened[T_MODE] = true;
						if (_last != mp.first || _modsToSend.empty())
						{
							_last = mp.first;
							_modsToSend += type_to_c(_last);
						}
						_modsToSend += 't';
					}
					mods.pop();
				}
				if (!_modsToSend.empty())
				{
					std::string fullMods = _modsToSend + " " + _paramsToSend;
					channel->sendToChannel(NULL, _messageCodes.sendModeChan(user->getFullname(), channel->getChannelName(), fullMods));
				}
			}
		}
	}
}
Oper::Oper(Server *server): Command(server) {  }
Oper::~Oper() {  }

void	Oper::execute(User *user, s_msg &msg)
{
	if (user->getStatus() != REGISTERED)	
		user->sendToClient(_messageCodes.errorNotRegistered(user->getNickname(), "OPER"));
	else if (msg.args.empty() || msg.args.size() != 2)
		user->setNickname(_messageCodes.errorNeedMoreParams(user->getNickname(), "OPER"));
	else if (msg.args[0] != OPLOGIN || msg.args[1] != OPPASS)
		user->sendToClient(_messageCodes.errorNoOperHost(user->getNickname()));
	else
	{
		user->setServerOP(true);
		user->sendToClient(_messageCodes.rplYoureOper(user->getNickname()));
		user->sendToClient(_messageCodes.sendModeUser(user->getFullname(), user->getNickname(), "+o"));
	}
}

Poweroff::Poweroff(Server *server): Command(server) {  }
Poweroff::~Poweroff() {  }

void	Poweroff::execute(User *user, s_msg &msg)
{
	(void)msg;
	if (user->getStatus() != REGISTERED)
		user->sendToClient(_messageCodes.errorNotRegistered(user->getNickname(), "POWEROFF"));
	else if (!user->isServerOp())
		user->sendToClient(_messageCodes.errorNoPrivileges(user->getNickname()));
	else
		_server->shutdown();
}
