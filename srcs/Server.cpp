#include "Server.hpp"

bool	stopSignalReceived = false;

void	handleSignal(int signal)
{
	MessageCodes _messageCodes;
	(void)signal;
	_messageCodes.msg_log(_messageCodes.msgServerExitSignal());
	stopSignalReceived = true;
}

Server::Server(std::string const &port, std::string const &password):
	_password(password), _nbOfClients(0)
{
	setPort(port);
	memset(&_addrServer, 0, sizeof(_addrServer));
	initCommands(this, _commands);
	_serverSocket = _messageCodes.getError();
	_messageCodes.msg_log(_messageCodes.msgServerCreated());
}

Server::~Server()
{
	_messageCodes.msg_log(_messageCodes.msgServerShutdown());
	disconnectAllClients();
	deleteCommands(_commands);
	_messageCodes.msg_log(_messageCodes.msgServerEnd());
}

void Server::setPort(std::string const &port)
{
    std::stringstream ss(port);
    long int tmp_port;
    ss >> tmp_port;

    if (ss.fail() || !ss.eof()) {
        throw std::invalid_argument("unvalid port: " + port);
    }

    if (tmp_port < 0 || tmp_port > _messageCodes.getPortMax()) {
        throw std::out_of_range("Port out of valid range: " + port);
    }

    if (tmp_port < _messageCodes.getPortMin()) {
        throw std::out_of_range("Port is forbidden: " + port);
    }

    _port = (unsigned int)tmp_port;

    std::string stringPort = _messageCodes.to_string(_port);
    _messageCodes.msgDev(_messageCodes.msgDevPortIsSet(), stringPort);
}


void	Server::setServerSocket()
{
	if ((_serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == _messageCodes.getError())
		throw std::runtime_error("unable to create socket");
	std::string newServerSocket = _messageCodes.to_string(_serverSocket);
	_messageCodes.msgDev(_messageCodes.msgDevServerSocketCreated(), newServerSocket);

	int	enable = 1;
	if (setsockopt(_serverSocket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) == _messageCodes.getError())
		throw std::runtime_error("unable to configure socket to 'reusable'");
	_messageCodes.msgDev(_messageCodes.msgDevServerSocketSetOpt(), newServerSocket);

	_addrServer.sin_family = AF_INET;
	_addrServer.sin_addr.s_addr = INADDR_ANY;
	_addrServer.sin_port = htons(_port);
	std::string stringServerIp = _messageCodes.to_string(inet_ntoa(_addrServer.sin_addr));
	_messageCodes.msgDev(_messageCodes.msgDevServerIpSetup(), stringServerIp);

	if (fcntl(_serverSocket, F_SETFL, O_NONBLOCK) == _messageCodes.getError())
		throw std::runtime_error("unable to set socket to non blocking mode");
	std::string empty = _messageCodes.to_string("");
	_messageCodes.msgDev(_messageCodes.msgDevServerSocketNonBlock(), empty);

	if (bind(_serverSocket, (const struct sockaddr *)&_addrServer, sizeof(_addrServer)) == _messageCodes.getError())
		throw std::runtime_error(std::string("unable to bind server socket to address") + strerror(errno));
	_messageCodes.msgDev(_messageCodes.msgDevServerSocketIpv4Binded(), empty);

	if (listen(_serverSocket, _messageCodes.getMaxClients()) == _messageCodes.getError())
		throw std::runtime_error("unable to enable listening mode for the server socket");
	_messageCodes.msgDev(_messageCodes.msgDevServerSocketListen(), empty);
}

void	Server::handlePollEvents()
{
	if (poll(_fds.data(), _fds.size(), _messageCodes.getTimeout()) == _messageCodes.getError() && stopSignalReceived == false)
		throw std::runtime_error("poll error: " + std::string(strerror(errno)));

	for (size_t i = 0; i < _fds.size(); i++)
	{
		if (_fds[i].revents & POLLIN)
		{
			if (_fds[i].fd == _serverSocket)
				handleNewConnection();
			else
				handleIncomingData(_fds[i].fd);
		}
	}
}

void	Server::handleNewConnection()
{
	sockaddr_in		clientAddr;
	memset(&clientAddr, 0, sizeof(clientAddr));
	socklen_t		clientAddr_len = sizeof(clientAddr);
	int	clientSocket = accept(_serverSocket, (struct sockaddr*)&clientAddr, &clientAddr_len);
	if (clientSocket == _messageCodes.getError())
	{
		_messageCodes.msgErr(strerror(errno));
		return ;
	}

	char	clientHostname[NI_MAXHOST];

	int		res = getnameinfo((struct sockaddr *) &clientAddr, sizeof(clientAddr), clientHostname, NI_MAXHOST, NULL, 0, NI_NUMERICSERV);
	if (res != 0)
	{
		close(clientSocket);
		_messageCodes.msgErr(strerror(errno));
		return ;
	}
	addToPoll(clientSocket, false);
	User	*newUser = new User(this, clientSocket, clientHostname);
	_users[clientSocket] = newUser;
	_messageCodes.msg_log(_messageCodes.msgClientConnected(clientSocket));
	newUser->setStatus(CONNECTED);
}

void	Server::handleIncomingData(int &clientfd)
{
	int					bytesReceived;
	char				buffer[_messageCodes.getBufferSize()];
	std::vector<s_msg>	fullMsg;
	User	*user = _users.find(clientfd)->second;
	static std::string	incompleteLine;
	
	memset(buffer, 0, sizeof(buffer));
	bytesReceived = recv(clientfd, buffer, sizeof(buffer) -1, 0);
	if (bytesReceived == _messageCodes.getError() || bytesReceived == 0 || bytesReceived >= 512)
	{
		disconnectClient(user);
		return ;
	}
	else
	{
		buffer[bytesReceived] = '\0';
		incompleteLine += buffer;

		if (!incompleteLine.empty() && incompleteLine[incompleteLine.size() - 1] == '\n')
		{
			parseIncomingData(incompleteLine, fullMsg);
			for (msg_iterator it = fullMsg.begin(); it != fullMsg.end(); it++)
			{
				execute(this, user, *it);
			}
			incompleteLine.clear();
		}
	}
}

void	Server::parseIncomingData(std::string const &buffer, std::vector<s_msg> &fullMsg)
{
	std::istringstream	iss(buffer);
	std::string			line;

	while (std::getline(iss, line))
	{
		if (!line.empty())
		{
			s_msg	parsedLine = parseLine(line);
			fullMsg.push_back(parsedLine);
		}
	}
}

s_msg	Server::parseLine(std::string const &line)
{
	s_msg				parsedMsg;
	std::istringstream	iss(line);
	//:NickName PRIVMSG #channel :Hello there

	parsedMsg.prefix = "";
	parsedMsg.cmd = "";
	parsedMsg.args.clear();
	parsedMsg.trailing = "";
	parsedMsg.trailing_sign	= false;

	if (line[0] == ':')
	{
		size_t	prefixEnd = line.find(' ');
		if (prefixEnd != std::string::npos)
		{
				parsedMsg.prefix = line.substr(1, prefixEnd - 1);
				iss.ignore(prefixEnd + 1);
		}
		else
			return (parsedMsg);
	}

	iss >> parsedMsg.cmd;

	std::string	arg;
	while (iss >> arg)
	{
		if (arg[0] == ':')
		{
			parsedMsg.trailing_sign = true;
			parsedMsg.trailing = arg.substr(1);
			
			while (iss >> arg)
			{
				parsedMsg.trailing += ' ' + arg;
			}
			break ;
		}
		else
			parsedMsg.args.push_back(arg);
	}
	return (parsedMsg);
}

void	Server::start()
{
	_messageCodes.msg_log(_messageCodes.msgServerStart());
	signal(SIGINT, handleSignal);
	signal(SIGPIPE, SIG_IGN);
	setServerSocket();
	addToPoll(_serverSocket, true);
	_messageCodes.msg_log(_messageCodes.msgServerStarted());
	while (stopSignalReceived == false)
		handlePollEvents();
}

void	Server::shutdown() { stopSignalReceived = true; }

void	Server::disconnectClient(User *client)
{
	int	clientFD = client->getSocketFd();
	client->leaveAllChannels("QUIT");
	deleteFromPoll(clientFD);
	deleteUser(clientFD);
	close(clientFD);
	delete client;
	_messageCodes.msg_log(_messageCodes.msgClientDisconnected(clientFD));
}

void	Server::disconnectAllClients()
{
	client_iterator it = _users.begin();
	while (it != _users.end())
	{
		client_iterator	current = it;
		it++;
		User	*client =  current->second;
		client->sendToClient(_messageCodes.msgClientServerShutdown());
		disconnectClient(client);
	}
}

void	Server::newChannel(std::string const &name, User *user)
{
	Channel	*chan = new Channel(this, name, user);

	_channels.push_back(chan);
}

void	Server::newChannel(std::string const &name, User *user, std::string const &key)
{
	Channel	*chan = new Channel(this, name, user, key);

	_channels.push_back(chan);
}

void	Server::deleteChannel(Channel *channel)
{
	for (channel_iterator it = _channels.begin(); it != _channels.end();)
	{
		if (*it == channel)
		{
			delete *it;
			it = _channels.erase(it);
			break ;
		}
		else
			it++;
	}
}

Channel	*Server::findChannel(std::string const &name)
{
	if (name.empty())
		return (NULL);
	for (channel_iterator it = _channels.begin(); it != _channels.end(); it++)
	{
		if ((*it)->getChannelName() == name)
			return (*it);
	}
	return (NULL);
}

void	Server::addToPoll(int fd, bool isServer)
{
	pollfd	newPoll;

	newPoll.fd = fd;
	newPoll.events = POLLIN;
	newPoll.revents = 0;
	_fds.push_back(newPoll);
	if (!isServer)
		_nbOfClients++;
}

void	Server::deleteFromPoll(int fd)
{
	for (pollfd_iterator it = _fds.begin(); it != _fds.end(); ++it)
	{
		if (it->fd == fd)
		{
			_fds.erase(it);
			_nbOfClients--;
			break ;
		}
	}
}

void	Server::deleteUser(int fd)
{
	client_iterator it = _users.find(fd);

	if (it != _users.end())
		_users.erase(it);
}

User	*Server::getUserWithNickname(std::string const &nickname)
{
	for (client_iterator it = _users.begin(); it != _users.end(); it++)
	{
		if (!nickname.compare(it->second->getNickname()))
			return (it->second);
	}
	return (NULL);
}

std::string const &Server::getPassword() const { return _password; }

std::map<std::string, Command *> &Server::getCommands() { return _commands; }

User	*Server::getUserwithFd(int client_socket)
{
	for (client_iterator it = _users.begin(); it != _users.end(); it++)
	{
		if (client_socket == it->first)
			return (it->second);
	}
	return (NULL);
}
