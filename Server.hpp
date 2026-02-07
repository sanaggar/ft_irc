#ifndef SERVER_HPP
#   define SERVER_HPP

#   include "Channel.hpp"
#   include "User.hpp"
#   include "defines.hpp"
#   include "debugMessages.hpp"

/*************************************************/
struct TempUserInfo
{
    std::string pass;
    std::string nick;
    std::string user;
    std::string host;
    std::string realname;
    bool hasPass;
    bool hasNick;
    bool hasUser;
};

extern std::map<int, TempUserInfo> tempUserInfo;

/**********************************************/


class                           Server
{
	private:
        char					_buffer[1024];
        map<int, string> 		_buffer_sd;
        map<string, Channel*>   _channels;
        struct sockaddr_in		_cliAddr;
        socklen_t 				_cliLen;
        string					_newbuff;
        int						_newClientSocket;
        string					_password;
        string					_port;
        int						_portnb;
        struct sockaddr_in		_servAddr;
        string					_serverName;
        int						_serverSocket;
        string					_strbuffer;
        map<int, User*>			_users;
        
        void					_sendMessage(string message, int sd);

    public:
        char				    **av;
        struct pollfd		    clientfd[MAX_CLIENTS + 1];

        Server(const string& port, const string& password);
        ~Server();

        void                	acceptClientconnexion();
		void				    add_user_to_channel(const string &channelName, int fd);
        void				    check_connection();
		void	            	clear_buffers();
		int					    count_channels(const string &channelsList);
		void				    create_channel(const string &channelName);
		void				    create_and_add_user(const string &nick,
                                    const string &user, const string &host,
                                    const string &realName, int client_socket);
        void                    error(const char *msg);
		string		            extract_channels_list(const string &buf);
		string		            extract_channel_name(const string &buffer);
		string			        extract_channel_name1(const string &buf);
		string		            extract_keys_list(const string &buf);
		string			        extract_next_channel(string &channelsList);
		string			        extract_next_key(string &keysList);
		string			        extract_user_nickname(const string &buf);
		string			        extract_topic(const string &buffer);
        User*				    find_user(int fd);
        User*			        find_user_byfd(int fd);
        bool				    firstConnection(int i);
		string			        get_channels_name(const string &buf);
		void	            	handle_channel_message(const string& channel_name,
                                    const string& sender_message, int sender_fd);
		void				    handle_kick_user_from_channel(const string &userNick,
                                    const string &channelName, int fd);
		void				    handle_part_channels(string &channelname,
                                    const string &msg, int i);
		void		            handle_private_message(const string& recipient_nickname,
                                    const string& sender_message, int sender_fd);
        void                    handling_server_msg(int i);
		void		            invite_user_to_channel(int userToSendSd,
                                    const string &channelName, int fd, const string &userNickname);
        bool				    isThereanUser(int fd);
		bool				    is_channel_empty(const string &channelName);
		bool				    is_channel_full(Channel *channel, int fd);
		bool				    is_channel_non_existent(const string &channelName);
		bool				    is_channel_valid(const string &channelName, int fd);
		bool            		is_invite_only_channel(const string &channelName, int fd);
		bool				    is_key_valid(Channel *channel, const string &key, int fd);
		bool				    is_user_allowed_in_channel(const string &channelName,
                                    const string &key, int fd);
		bool				    is_user_already_in_channel(const string &channelName, int fd);
		bool				    is_user_invited(Channel *channel, int fd);
		bool				    is_user_in_channel(int userSd, const string &channelName);
		bool		            is_user_not_in_channel(int fd, const string &channelName);
        bool				    nicknameAlreadyUse(string nick);
		void				    notify_channel_join(const string &channelName, int fd);
        void	    	        priv_msg(string buffer, int fd);
		void				    process_user_part(const string &chan_name,
                                    const string &msg, int i);
        void				    recvClientMsg(int i);
        int				        searchUserby_nickname(const string& nickname);
        void				    sendinchanexceptuser(string message, Channel *chan, int sd);
		void				    sendtoeveryone(const string &message, Channel *chan);
		void				    send_error_message(int fd, const string &command);
		void				    send_topic_to_user(const string &channelName, int fd);
		void				    send_user_list_to_user(const string &channelName, int fd);
		void				    send_channel_modes_to_user(const string &channelName, int fd);
        void				    setserversocket();
		void        			set_and_announce_topic(int fd,
                                    const string &channelName, const string &topic);
        void					user_disconnect(int sd);
		bool		            validate_channel_and_user(int fd, const string &channelName);
		bool				    validate_nickname(const string &ret,
                                    int client_socket, string &nick);
		bool			        validate_password(const string &ret, int client_socket);
		bool			        validate_user(const string &ret, int client_socket,
                                    string &user, string &host,
                                    string &serverName, string &realName);
       
        void				    invite_command(const string& buffer, int fd);
        void				    join_command(const string& tmp, int i);
        void		            kick_command(const string& buffer, int i);
        void		            nick_command(string buffer, int fd);
        void				    oper_command(string buffer, int fd);
        void 				    part_command(string tmp, int i);
        void            		pong_command(string tmp, int i);
		void				    process_kick_command(string &buf,
                                    string &usersNick, int nbUsers,
                                    string &channelsName, int nbOfChannels, int fd);
        void				    topic_command(string buffer, int sd);

		void		            add_mode(Channel *channel, string mode, int fd, string buffer);
        void				    channel_mode(Channel *channel, string mode, int fd, string buffer);
        void        	    	mode_channel_handler(string buffer, int fd);
        void			        mode_k_command(Channel *channel, string mode, string buffer, int fd);
        void				    mode_l_command(Channel *channel, string mode, string buffer, int fd);
        void	            	mode_o_command(Channel *channel, string mode, string tmp, int fd);
		void		    	    remove_modes(Channel *channel, string mode, int fd, string buffer);
       

        map<int, string>		&getBufferSd();
        map<string, Channel*>	&getChannels();
        const string&			getPort();
        const string&			getServername();
        string&	        		get_bufferstr();
        struct pollfd		    get_clientfd();
        int					    get_newClientSocket();
        map<int, User*>			&get_users();
		string                  get_users_nick(const string &buf, size_t &k);

        void				    setBufferSd(int sd, string buffer);
        void				    setChannels(string name, Channel *channel);
        void				    setUsers(int fd, User *user);
        void			        setServername(string servername);
        void					set_bufferstr(string str);

		/*********************/
		void createUserFromTempInfo(int fd, TempUserInfo &info);
		void handleUserCommand(TempUserInfo &info, const std::string &command);
		void handleNickCommand(TempUserInfo &info, const std::string &command);
		void handlePassCommand(TempUserInfo &info, const std::string &command);

};


bool		                    channelNameInvalid(string name);
bool		                    checkInvalidCharacter(char c);
string                          getCurrentDate();
bool		                    nicknameIsValid(string nick);
string                          print_user(User *usr);
void		                    removeSpaces(string &str);
bool	                        running_status();
string	                        send_codes(int code, Server *serv, User *usr, string buf1, string buf2);


#endif
