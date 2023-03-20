/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client_ops.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sel-mars <sel-mars@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/20 17:01:48 by sel-mars          #+#    #+#             */
/*   Updated: 2023/03/20 17:16:37 by sel-mars         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../irc.hpp"

/* find_client_by_nickname ──────────────────────────────────────────────────────────── */
bool irc::server::findClientByNick( const std::string& nick_ ) {
	irc::server::client_iterator cl_it;
	for ( cl_it = this->_clients.begin();
		  cl_it != this->_clients.end() && cl_it->second._nickname.compare( nick_ ); cl_it++ )
		continue;
	return cl_it != this->_clients.end();
} // find_client_by_nickname

/* accept_client ──────────────────────────────────────────────────────────────────── */

void irc::server::acceptClient( void ) {
	pollfd pfd;
	bzero( &pfd, sizeof( pfd ) );
	pfd.fd = accept( this->_sockets.front().fd, NULL, NULL );
	if ( pfd.fd == -1 ) return;
	pfd.events = POLLIN | POLLOUT;
	this->_sockets.push_back( pfd );
	this->_clients.insert( std::make_pair( pfd.fd, irc::client( this->_sockets.back() ) ) );
} // accept_client

/* diconnect_client ───────────────────────────────────────────────────────────────── */

void irc::server::disconClient( client_iterator& client_it_ ) {
	close( client_it_->second._pfd.fd );
	std::cout << "\033[2m"
			  << "User `" << client_it_->second._nickname << "` disconnected"
			  << "\033[22m\n";
	std::vector< pollfd >::iterator pfd_it = this->_sockets.begin();
	while ( pfd_it->fd != client_it_->second._pfd.fd ) pfd_it++;
	this->_sockets.erase( pfd_it );
	for ( channel_iterator channel_it = this->_channels.begin(); channel_it != _channels.end();
		  ++channel_it )
		channel_it->second->_members.erase( &client_it_->second );
	this->_clients.erase( client_it_ );
	client_it_ = this->_clients.begin();
} // diconnect_client
