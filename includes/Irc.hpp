#ifndef IRC_HPP
# define IRC_HPP

#ifndef OPLOGIN
    #define OPLOGIN "admin"
#endif

#ifndef OPPASS
    #define OPPASS "pass"
#endif

# include <iostream>
# include <string>
# include <cstdlib>
# include <climits>
# include <cerrno>
# include <cstring>
# include <sstream>
# include <algorithm>
# include <sys/socket.h>
# include <sys/poll.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <netdb.h>
# include <vector>
# include <map>
# include <list>
# include <queue>
# include <ctime>
# include <fcntl.h>
# include <csignal>
# include <unistd.h>

struct	s_msg
{
	std::string					prefix; 
	std::string					cmd; 
	std::vector<std::string>	args; 
	std::string					trailing;
	bool						trailing_sign; 
};
typedef uint8_t channelModes; 

static const channelModes INVITE_ONLY = 0x01; 
static const channelModes KEY = 0x02;         
static const channelModes LIMIT = 0x04;       
static const channelModes TOPIC = 0x08;

enum	userLevel { INVITED, NORMAL, OPERATOR };
enum	modeType { PLUS, MINUS };
enum	modeVal { BAD_MODE = -1, I_MODE, K_MODE, L_MODE, O_MODE, T_MODE }; 
typedef	std::pair<modeType, modeVal> modePair; 

class User;
class Channel;
typedef std::vector<pollfd>::iterator		pollfd_iterator;
typedef std::map<int, User *>::iterator		client_iterator;
typedef std::vector<s_msg>::iterator		msg_iterator;
typedef std::vector<Channel *>::iterator	channel_iterator;

# include "MessageCodes.hpp"
# include "Server.hpp"
# include "User.hpp"
# include "Commands.hpp"
# include "Channel.hpp"

#endif