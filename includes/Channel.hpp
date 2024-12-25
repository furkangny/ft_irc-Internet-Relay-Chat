#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include "Irc.hpp"

typedef std::list<User*>::iterator	user_iterator;
class User;

class Channel
{
	private:
		Server				*_server;
		std::string			_channelName;
		channelModes		_modes;
		std::string			_topic;
		std::string			_password;
		int					_maxUsers;
		std::list<User*>	_operators;
		std::list<User*>	_normalUsers;
		std::list<User*>	_invitedUsers;
        MessageCodes 		_messageCodes;
		User				*findUserFromList(std::list<User *> &role, std::string nickname);
		bool				isChannelEmpty();

		Channel();
		Channel(Channel const &toCopy);
	public:
		Channel(Server *server, std::string name, User *user);
		Channel(Server *server, std::string name, User *user, std::string const &password);
		~Channel();
		std::string	const	&getChannelName() const;
		std::string	const	&getPassword()const;
		std::string	const	&getTopic() const;
		int					getMaxUsers() const;
		bool				isInvited(std::string const &nickname);
		bool				isOperator(std::string const &nickname);
		bool				isNormal(std::string const &nickname);
		int					getTotalUsers() const;
		bool				isPasswordCorrect(std::string const &pass);
		void				setPassword(std::string const &password);
		void				setTopic(std::string const &topic);
		void				setMaxUsers(int max);
		void				setMode(channelModes mode, modeType type);
		bool				isMode(channelModes mode) const;
		std::string			listModes(bool isMember) const;
		void				addUser(User *user, userLevel lvl);
		void				removeUser(User *user);
		void				setUserLevel(User *user, userLevel lvl);
		User				*findUserInChannel(std::string nickname);
		void				displayUsers(User *);
		void				welcomeUser(User *user);
		void				sendToChannel(User *user, std::string const &msg);
		std::string const	sendTopic(User *user) const;
};

#endif
