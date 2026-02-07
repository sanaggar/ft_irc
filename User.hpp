#ifndef USER_HPP
# define USER_HPP

#include "defines.hpp"

class               User
{
    private:
        set<string> _channels;
        string      _host;
        string      _mode;
        string      _nick;
        string      _realname;
        string      _user;
		bool authenticated;

    public:
        User(string nick, string user, string host, string realname);
        ~User();

        void        addchannel(string name);
        size_t      ls_channel();

        set<string> &get_channels();
        string      get_hostname();
        string      get_mode();
        string      get_nickname();
        string      get_realname();
        string      get_username();

        void        sethost(string name);
        void        setnickname(string name);
        void        setuser(string name);
        void        setrealname(string name);
        void        set_mode(string mode);

		void		authenticate();
    	bool		isAuthenticated() const;
};

#endif
