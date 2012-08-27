/*
    Copyright (c) 2012 Linkin Oleg <MaledictusDeMagog@gmail.com>

    Permission is hereby granted, free of charge, to any person
    obtaining a copy of this software and associated documentation
    files (the "Software"), to deal in the Software without
    restriction, including without limitation the rights to use,
    copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following
    conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
    OTHER DEALINGS IN THE SOFTWARE.
*/

#include <fstream>
#include <string>
#include <Swiften/Swiften.h>
#include "vortexbot.h"

namespace XmppBot
{
	VortexBot::VortexBot (const std::string& configFilePath)
	{
		boost::program_options::options_description desc("Config file options");
		desc.add_options()
			("login.jid", boost::program_options::value<std::string> ())
			("login.password", boost::program_options::value<std::string> ())
			("autojoin.room", boost::program_options::value<std::vector<std::string> >());
		std::ifstream configFile (configFilePath);
		boost::program_options::store (boost::program_options::parse_config_file (configFile, desc),
				VM_);
		boost::program_options::notify (VM_);
	}

	VortexBot::~VortexBot ()
	{
		delete Client_;
	}

	void VortexBot::Connect ()
	{
		Swift::SimpleEventLoop loop;
		Swift::BoostNetworkFactories factories (&loop);

		Client_ = new Swift::Client ("jid",
				"pass",
				&factories);

		Client_->setAlwaysTrustCertificates ();
		Client_->onConnected.connect ([=] () { onConnected (); });
		Client_->onMessageReceived.connect ([=] (Swift::Message::ref msg) { onMessageReceived (msg); });
		Client_->connect ();

		loop.run ();
	}

	void VortexBot::onConnected ()
	{
		std::cout << "connected" << std::endl;
		MUC_ = Client_->getMUCManager ()->createMUC (Swift::JID ("leechcraft@conference.jabber.ru"));
		MUC_->joinAs ("maledictus");
		MUC_->onJoinComplete.connect ([=] (const std::string& str) { onMucJoin (str); });
	}

	void VortexBot::onMessageReceived (Swift::Message::ref msg)
	{
		msg->setTo (msg->getFrom ());
		msg->setFrom(Swift::JID ());
		Client_->sendMessage (msg);
	}

	void VortexBot::onMucJoin (const std::string& str)
	{
		std::cout << str << std::endl;
	}

}