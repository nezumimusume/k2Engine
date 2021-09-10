/* Exit Games Photon LoadBalancing - C++ Client Lib
 * Copyright (C) 2004-2021 by Exit Games GmbH. All rights reserved.
 * http://www.photonengine.com
 * mailto:developer@photonengine.com
 */

#include "LoadBalancing-cpp/inc/ConnectOptions.h"

/** @file LoadBalancing-cpp/inc/ConnectOptions.h */

namespace ExitGames
{
	namespace LoadBalancing
	{
		using namespace Common;

		const EG_CHAR* ConnectOptions::M_NAMESERVER = L"ns.photonengine.io"; // default name server address
		
		/** @class ConnectOptions
		   This class aggregates the various optional parameters that can be passed to Client::connect().
		   @sa Client::connect() */
		
		/**
		   Constructor: Creates a new instance with the specified parameters.
		   @note on XBox consoles the value of parameter 'tryUseDatagramEncryption' is ignored and the client always behaves as if it was 'true'.
		   @param authenticationValues An instance of class AuthenticationValues - optional, defaults to a default-constructed AuthenticationValues instance.
		   @param username The users display name as shown to other users - not to be confused with the users unique ID for identification and authentication purposes, which is part of the
		   AuthenticationValues - optional, defaults to an empty string.
		   @param serverAddress A null terminated string containing the IP address or domain name and optionally the port number to connect to. IP addresses can be in IPv4 or IPv6 format,
		   examples: L"192.168.0.1", L"192.168.0.1:5055", L"udp.gameserver.com", L"udp.gameserver.com:5055", L"[2002:C0A8:1::]", L"[2002:C0A8:1::]:5055". Note that IPv6 addresses must include square
		   brackets to indicate where the address itself ends and the port begins. If no port is given, then the default port for the chosen protocol and server type will be used. - optional, defaults
		   to the address of Photon Cloud.
		   @param serverType One of the values in ServerType. Must match the type of the %Photon server that is reachable at the given address and port. Should be ServerType::NAME_SERVER for the
		   %Photon Cloud and for self-hosted %Photon 5 or higher Server instances and ServerType::MASTER_SERVER for self-hosted %Photon 4 Server instances. You should NOT directly pass the address of a
		   regions master server with %Photon Cloud or with Photon 5 Server and up, but you should always connect to the name server when you connect to Photon Cloud or to Photon 5 Server and up. - optional,
		   defaults to ServerType::NAME_SERVER.
		   @param tryUseDatagramEncryption defaults to 'false'. Setting this to 'true' results in encryption-key exchange via an initial WSS-connection and in all UDP traffic getting encrypted, if all of
		   the following is true: the argument for 'serverType' is ServerType::NAME_SERVER, the connection protocol specified in the constructor is ConnectionProtocol::UDP, the Photon Client for the platform
		   supports UDP encryption and the Photon client for the platform supports ConnectionProtocol::WSS. If any of these conditions is false, then the value of this parameter is ignored and the client behaves
		   as if it was 'false'. - optional defaults to false. */
		ConnectOptions::ConnectOptions(const AuthenticationValues& authenticationValues, const JString& username, const JString& serverAddress, nByte serverType, bool tryUseDatagramEncryption)
			: mAuthenticationValues(authenticationValues)
			, mUsername(username)
			, mServerAddress(serverAddress)
			, mServerType(serverType)
			, mTryUseDatagramEncryption(tryUseDatagramEncryption)
		{
		}
		
		/**
		   Destructor. */
		ConnectOptions::~ConnectOptions(void)
		{
		}
		
		/**
		   Copy-Constructor: Creates a new instance that is a deep copy of the argument instance.
		   @param toCopy The instance to copy. */
		ConnectOptions::ConnectOptions(const ConnectOptions& toCopy)
		{
			*this = toCopy;
		}
		
		/**
		   operator=.
	   
		   Makes a deep copy of its right operand into its left operand.
	   
		   This overwrites old data in the left operand. */
		ConnectOptions& ConnectOptions::operator=(const ConnectOptions& toCopy)
		{
			mAuthenticationValues = toCopy.mAuthenticationValues;
			mUsername = toCopy.mUsername;
			mServerAddress = toCopy.mServerAddress;
			mServerType = toCopy. mServerType;
			mTryUseDatagramEncryption = toCopy.mTryUseDatagramEncryption;

			return *this;
		}

		/**
		   @returns the currently set AuthenticationValues
		   @sa
		   setAuthenticationValues() */
		const AuthenticationValues& ConnectOptions::getAuthenticationValues(void) const
		{
			return mAuthenticationValues;
		}
		
		/**
		   Sets the AuthenticationValues.
		   @details
		   Please see AuthenticationValues for further information.
		   @param authenticationValues An instance of class AuthenticationValues.
		   @returns a reference to the instance on which it was called to allow for chaining multiple setter calls
		   @sa
		   getAuthenticationValues(), AuthenticationValues */
		ConnectOptions& ConnectOptions::setAuthenticationValues(const AuthenticationValues& authenticationValues)
		{
			mAuthenticationValues = authenticationValues;
			return *this;
		}
		
		/**
		   @returns the currently set user name
		   @sa
		   setUsername() */
		const JString&  ConnectOptions::getUsername(void) const
		{
			return mUsername;
		}
		
		/**
		   Sets the user name.
		   @details
		   Please see AuthenticationValues for further information.
		   @param username The users display name as shown to other users - not to be confused with the users unique ID for identification and authentication purposes, which is part of the
		   AuthenticationValues.
		   @returns a reference to the instance on which it was called to allow for chaining multiple setter calls
		   @sa
		   getUsername() */
		ConnectOptions& ConnectOptions::setUsername(const JString& username)
		{
			mUsername = username;
			return *this;
		}
		
		/**
		   @returns the currently set server address
		   @sa
		   setServerAddress() */
		const JString& ConnectOptions::getServerAddress(void) const
		{
			return mServerAddress;
		}
		
		/**
		   Sets the server address.
		   @param serverAddress A null terminated string containing the IP address or domain name and optionally the port number to connect to. IP addresses can be in IPv4 or IPv6 format,
		   examples: L"192.168.0.1", L"192.168.0.1:5055", L"udp.gameserver.com", L"udp.gameserver.com:5055", L"[2002:C0A8:1::]", L"[2002:C0A8:1::]:5055". Note that IPv6 addresses must include square
		   brackets to indicate where the address itself ends and the port begins. If no port is given, then the default port for the chosen protocol and server type will be used. Defaults to the address
		   of Photon Cloud.
		   @returns a reference to the instance on which it was called to allow for chaining multiple setter calls
		   @sa
		   getServerAddress() */
		ConnectOptions& ConnectOptions::setServerAddress(const JString& serverAddress)
		{
			mServerAddress = serverAddress;
			return *this;
		}
		
		/**
		   @returns the currently set ServerType
		   @sa
		   setServerType() */
		nByte ConnectOptions::getServerType(void) const
		{
			return mServerType;
		}
		
		/**
		   Sets the ServerType.
		   @param serverType One of the values in ServerType. Must match the type of the %Photon server that is reachable at the given address and port. Should be ServerType::NAME_SERVER for the
		   %Photon Cloud and for self-hosted %Photon 5 or higher Server instances and ServerType::MASTER_SERVER for self-hosted %Photon 4 Server instances. You should NOT directly pass the address of a
		   regions master server with %Photon Cloud or with Photon 5 Server, but you should always connect to the name server when you connect to Photon Cloud or to Photon 5 Server and up.
		   @returns a reference to the instance on which it was called to allow for chaining multiple setter calls
		   @sa
		   getServerType() */
		ConnectOptions& ConnectOptions::setServerType(nByte serverType)
		{
			mServerType = serverType; 
			return *this;
		}
		
		/**
		   @returns the currently set value for the tryUseDatagramEncryption flag
		   @sa
		   setServerType() */
		bool ConnectOptions::getTryUseDatagramEncryption(void) const
		{
			return mTryUseDatagramEncryption;
		}

		/**
		   Sets the value for the tryUseDatagramEncryption flag.
		   @note on XBox consoles the value of parameter 'tryUseDatagramEncryption' is ignored and the client always behaves as if it was 'true'.
		   @param tryUseDatagramEncryption defaults to 'false'. Setting this to 'true' results in encryption-key exchange via an initial WSS-connection and in all UDP traffic getting encrypted, if all of
		   the following is true: the argument for 'serverType' is ServerType::NAME_SERVER, the connection protocol specified in the constructor is ConnectionProtocol::UDP, the Photon Client for the platform
		   supports UDP encryption and the Photon client for the platform supports ConnectionProtocol::WSS. If any of these conditions is false, then the value of this parameter is ignored and the client behaves
		   as if it was 'false'.
		   @returns a reference to the instance on which it was called to allow for chaining multiple setter calls
		   @sa
		   getTryUseDatagramEncryption() */
		ConnectOptions& ConnectOptions::setTryUseDatagramEncryption(bool tryUseDatagramEncryption)
		{
			mTryUseDatagramEncryption = tryUseDatagramEncryption;
			return *this;
		}

		JString& ConnectOptions::toString(JString& retStr, bool withTypes) const
		{
			return retStr += JString(L"{authenticationValues=") + getAuthenticationValues().toString(withTypes) + L", username=" + getUsername().toString(withTypes) + L", serverAddress=" + getServerAddress().toString(withTypes) + L", serverType=" + getServerType() + L", tryUseDatagramEncryption=" + getTryUseDatagramEncryption() + L"}";
		}
	}
}