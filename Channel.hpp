#ifndef CHANNEL_HPP
# define CHANNEL_HPP

#include "User.hpp"

class					Channel
{
	private:
		map<int, User*>	_chanUserOps;
		vector<int>		_invited;
		string			_key;
		int				_maxUser;
		string			_mode;
		string			_name;
		string			_topic;
		map<int, User*>	_users;

	public:
		Channel(string name);
		~Channel();

		void			addChanops(int fd, User *usr);
		void			addUser(int fd, User *usr);
		bool			isChanop(int fd);
		void			rm_user(int fd);
		int				searchuserbyname(string nickname);
		void			set_invited(int fd);

		map<int, User*>	&getChanOps();
		map<int, User*>	&getUsers();
		int				getUsersNb();
		string			get_channelname();
		vector<int>		get_invited();
		string			get_key();
		int				get_maxUser();
		string			get_mode();
		string			get_topic();
		string			get_userlistinchan();

		void			set_key(string key);
		void			set_maxUser(int maxUser);
		void			set_mode(string mode);
		void			set_topic(string topic);
};


#endif
