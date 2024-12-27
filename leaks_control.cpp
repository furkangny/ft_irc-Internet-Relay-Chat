void __attribute__((destructor)) checkleaks(void)
{
	system("leaks ircserv");
}
