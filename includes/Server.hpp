#ifndef SERVER_HPP
# define SERVER_HPP

# include "Irc.hpp"
# include "MessageCodes.hpp"

class User;
class Command;
class Channel;

class Server
{
	private:
		unsigned int		_port;
		const std::string	_password;
		int					_serverSocket;
		struct sockaddr_in	_addrServer;
		std::vector<pollfd>	_fds;
		int					_nbOfClients;

		std::map<int, User *>				_users;
		std::map<std::string, Command *>	_commands;
		std::vector<Channel *>				_channels;
        MessageCodes _messageCodes;

		void	setPort(std::string const &port);
		void	setServerSocket();
		void	handlePollEvents();
		void	handleNewConnection();
		void	handleIncomingData(int &clientfd);
		void	addToPoll(int fd, bool isServer);
		void	deleteFromPoll(int fd);
		void	deleteUser(int fd);
		void	disconnectAllClients();
		User	*getUserwithFd(int client_socket);
		void	parseIncomingData(std::string const &buffer, std::vector<s_msg> &fullMsg);
		s_msg	parseLine(std::string const &line);
		Server();								
		Server(Server &to_copy);

	public:
		Server(std::string const &port, std::string const &password);
		~Server();

		void	start();
		void	shutdown();
		void	disconnectClient(User *client);
		void	newChannel(std::string const &name, User *user);
		void	newChannel(std::string const &name, User *user, std::string const &key);
		void	deleteChannel(Channel *channel);
		Channel *findChannel(std::string const &name);

		std::string const					&getPassword() const;
		std::map<std::string, Command *>	&getCommands();
		User								*getUserWithNickname(std::string const &nickname);
};


#endif