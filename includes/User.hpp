#ifndef USER_HPP
# define USER_HPP

# include "Irc.hpp"
# include "MessageCodes.hpp"

enum	clientStatus
{
	CREATED,
	CONNECTED,
	PASSWORDACCEPTED,
	NICKNAMEISOK,
	USERNAMEISOK,
	REGISTERED
};

class Server;
class Channel;

class User
{
	private:
		Server			*_server;
		int				_socket_fd;
		clientStatus	_status;
		std::string     _nickname;
		std::string     _username;
		std::string     _realname;
		std::string		_hostname;
		std::string		_leavingMsg;
		bool			_op;
        MessageCodes _messageCodes;

		std::vector<Channel *>	_joinedChannels;
		std::vector<Channel *>	_invitedChannels;

		bool	isEmpty(std::string const &str) const;

		User();								
		User(User &to_copy);

	public:

		User(Server *server, int clientSocket, std::string const &clientHostname);
		~User();

		void	sendToClient(std::string const &msg);
		void	welcome();
		void	addJoinedChannel(Channel *channel);
		void	removeJoinedChannel(Channel *channel);
		void	addInvitedChannel(Channel *channel);
		void	removeInvitedChannel(Channel *channel);

		void	leaveChannel(Channel *channel, User *origin, std::string const &reason);
		void	leaveAllChannels(std::string const &reason);
		bool				isServerOp() const;
		int 				getSocketFd() const;
		clientStatus		getStatus() const;
		std::string	const	&getUsername() const;
		std::string	const	&getNickname() const;
		std::string	const	&getRealname() const;
		std::string	const	&getHostname() const;
		std::string			getFullname() const;
	
		void	setStatus(clientStatus status);
		void	setUsername(std::string const &username);
		void	setNickname(std::string const &nickname);
		void	setRealname(std::string const &realname);
		void	setHostname(std::string const &hostname);
		void	setLeavingMessage(std::string const &msg);
		void	setServerOP(bool val);
};

#endif

