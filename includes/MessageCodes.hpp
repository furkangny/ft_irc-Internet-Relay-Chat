#ifndef MESSAGECODES_HPP
# define MESSAGECODES_HPP

#include <iostream>
#include <map>
#include <string>
#include <sstream>

class MessageCodes {
    private:
        std::string _red;
        std::string _green;
        std::string _yellow;
        std::string _grey;
        std::string _no_color;
        std::string _svr_prefix;
        std::string _svr_name;
        
        int _error;
        int _max_clients;
        int _port_min;
        int _port_max;
        int _buffer_size;
        int _timeout;

    public:
        MessageCodes();

        // getter 
        const std::string& getRed() const;
        const std::string& getGreen() const;
        const std::string& getYellow() const;
        const std::string& getGrey() const;
        const std::string& getNoColor() const;
        const std::string& getSvrPrefix() const;
        int getError() const;
        const std::string& getSvrName() const;
        int getMaxClients() const;
        int getPortMin() const;
        int getPortMax() const;
        int getBufferSize() const;
        int getTimeout() const;

        void msg_log(const std::string& msg);

        // Server
        
        const std::string msgServerCreated();
        const std::string msgServerStart();
        const std::string msgServerStarted();
        const std::string msgServerWelcome();
        const std::string msgServerShutdown();
        const std::string msgServerEnd();
        const std::string msgServerExitSignal();


        // Client
        const std::string msgClientConnected(int socket);
        const std::string msgClientDisconnected(int socket);
        const std::string msgClientNick(int socket, const std::string& nick);
        const std::string msgClientUser(int socket, const std::string& name, const std::string& real);
        const std::string msgClientServerShutdown();
        const std::string msgClientQuit(const std::string& nick);

        template <typename T>
        std::string to_string(const T& value) const {
            std::ostringstream oss;
            oss << value;
            return oss.str();
        }

        // Dev
        const std::string msgDevPortIsSet();
        const std::string msgDevServerSocketCreated();
        const std::string msgDevServerSocketSetOpt();
        const std::string msgDevServerSocketNonBlock();
        const std::string msgDevServerIpSetup();
        const std::string msgDevServerSocketIpv4Binded();
        const std::string msgDevServerSocketListen();


        // Misc Messages
        const std::string sendNick(const std::string& full, const std::string& nick);
        const std::string sendInvite(const std::string& full, const std::string& nick, const std::string& chan);
        const std::string sendPm(const std::string& from, const std::string& to, const std::string& msg);
        const std::string sendPong(const std::string& nick);
        const std::string sendPart(const std::string& full, const std::string& chan);
        const std::string sendPartMsg(const std::string& full, const std::string& chan, const std::string& msg);
        const std::string sendJoin(const std::string& full, const std::string& chan);
        const std::string sendQuit(const std::string& full, const std::string& nick);
        const std::string sendQuitMsg(const std::string& full, const std::string& msg);
        const std::string sendKick(const std::string& full, const std::string& chan, const std::string& target, const std::string& nick);
        const std::string sendModeUser(const std::string& full, const std::string& nick, const std::string& mode);
        const std::string sendModeChan(const std::string& full, const std::string& chan, const std::string& mode);
        const std::string sendTopic(const std::string& full, const std::string& chan, const std::string& topic);
        
        // Channel
        const std::string msgChannelCreated(const std::string& nick, const std::string& chan);

        // irc
        const std::string errSvrUsage(const std::string& prog_name);

        // ERR
        const std::string errInvalidUsername(const std::string& username);
        const std::string errInvalidRealname(const std::string& realname);
        const std::string errBadChannelName(const std::string& nickname, const std::string& wrongname);
        const std::string errSyntax(const std::string& nickname, const std::string& command);

        // RPL_MSG_CODE
        const std::string rplWelcome(const std::string& nick, const std::string& full);
        const std::string rplYourHost(const std::string& nick);
        const std::string rplCreated(const std::string& nick);
        const std::string rplMyInfo(const std::string& nick);
        const std::string rplUModeIs(const std::string& nick, const std::string& modes);
        const std::string rplChannelModeIs(const std::string& nick, const std::string& chan, const std::string& mods);
        const std::string rplNoTopic(const std::string& nick, const std::string& chan) const;
        const std::string rplTopic(const std::string& nick, const std::string& chan, const std::string& topic) const;
        const std::string rplInviting(const std::string& nick, const std::string& invited, const std::string& chan);
        const std::string rplNameReply(const std::string& nick, const std::string& chan, const std::string& list);
        const std::string rplEndOfNames(const std::string& nick, const std::string& chan);
        const std::string rplYoureOper(const std::string& nick);
        
        // Error Messages
        const std::string errorNoSuchNick(const std::string& nick, const std::string& target);
        const std::string errorNoSuchChannel(const std::string& nick, const std::string& chan);
        const std::string errorCannotSendToChan(const std::string& nick, const std::string& chan);
        const std::string errorNoOrigin(const std::string& nick);
        const std::string errorNoRecipient(const std::string& nick);
        const std::string errorNoTextToSend(const std::string& nick);
        const std::string errorUnknownCommand(const std::string& nick, const std::string& cmd);
        const std::string errorNonicknameGiven(const std::string& nick);
        const std::string errorErroneusNickname(const std::string& nick, const std::string& wrong);
        const std::string errorNicknameInUse(const std::string& nick, const std::string& wrong);
        const std::string errorUserNotInChannel(const std::string& nick, const std::string& wrong, const std::string& chan);
        const std::string errorNotOnChannel(const std::string& nick, const std::string& chan);
        const std::string errorNotRegistered(const std::string& nick, const std::string& cmd);
        const std::string errorNeedMoreParams(const std::string& nick, const std::string& cmd);
        const std::string errorAlreadyRegistered(const std::string& nick);
        const std::string errorPasswdMismatch(const std::string& nick);
        const std::string errorChannelIsFull(const std::string& nick, const std::string& chan);
        const std::string errorUnknownMode(const std::string& nick, const std::string& mode);
        const std::string errorInviteOnlyChan(const std::string& nick, const std::string& chan);
        const std::string errorBadChannelKey(const std::string& nick, const std::string& chan);
        const std::string errorNoPrivileges(const std::string& nick);
        const std::string errorChanOpPrivsNeeded(const std::string& nick, const std::string& chan);
        const std::string errorNoOperHost(const std::string& nick);
        const std::string errorUModeUnknownFlag(const std::string& nick);
        const std::string errorUsersDontMatch(const std::string& nick);

        const std::string msgChannelDeleted(const std::string& chan);

        void msgDev(const std::string& msg, std::string& val);
        void msgErr(const std::string& msg);

};

#endif