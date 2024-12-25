# include "Irc.hpp"

int	main(int ac, char **av)
{
	MessageCodes _messageCodes;
	try
	{
		if (ac != 3)
		{
			throw std::invalid_argument(_messageCodes.errSvrUsage(std::string(av[0])));
		}
		Server	ircserv(av[1], av[2]);
		ircserv.start();
	}
	catch (const std::exception& e)
	{
		_messageCodes.msgErr(e.what());
		return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}
