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
#include <boost/algorithm/string.hpp>
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
			("autojoin.room", boost::program_options::value<std::string> ());
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

		if (!VM_.count ("login.jid") ||
				!VM_.count ("login.password"))
		{
			std::cout << "There is no JID or Password in config file" << std::endl;
			return;
		}

		Client_ = new Swift::Client (VM_ ["login.jid"].as<std::string> (),
				VM_ ["login.password"].as<std::string> (),
				&factories);

		Client_->setAlwaysTrustCertificates ();
		Client_->onConnected.connect ([=] () { onConnected (); });
		Client_->connect ();

		loop.run ();
	}

	void VortexBot::onConnected ()
	{
		std::cout << "connected" << std::endl;
		std::vector<std::string> rooms;
		if (VM_.count ("autojoin.room"))
		{
			std::string str = VM_ ["autojoin.room"].as<std::string> ();
			boost::split (rooms, str, boost::is_any_of (" "));
		}

		std::for_each (rooms.begin (), rooms.end (),
				[this] (decltype (rooms.front ()) room)
				{
					if (!JoinedMucs_.count (room))
					{
						Swift::MUC::ref muc = Client_->getMUCManager ()->createMUC (room);
						JoinedMucs_.insert ({ room, muc });
						muc->joinAs ("VortexBot");
						muc->onJoinComplete.connect ([this, room] (const std::string& str)
							{ onMucJoin (room, str); });
					}
				});
	}

	void VortexBot::onMucJoin (const std::string& room, const std::string& str)
	{
		if (!JoinedMucs_.count (room))
			return;

		std::cout << room << " joined" << std::endl;
		auto muc = JoinedMucs_ [room];
		muc->onOccupantJoined.connect ([this, room] (const Swift::MUCOccupant& occ)
			{ handleOccupantJoin (occ, room); });
		muc->onOccupantLeft.connect ([this, room]
				(const Swift::MUCOccupant& occupant,
						Swift::MUC::LeavingType type,
						const std::string& str)
					{ handleOccupantLeft (occupant, type, str, room); });
		muc->onOccupantRoleChanged.connect ([this, room]
				(const std::string& str,
						const Swift::MUCOccupant& occupant,
						const Swift::MUCOccupant::Role& role)
					{ handleOccupantRoleChanged (str, occupant, role, room); });
	}

	void VortexBot::handleOccupantJoin (const Swift::MUCOccupant& occupant,
			const std::string& room)
	{
		std::cout << occupant.getNick () << " occ joined" << std::endl;
		if (occupant.getRole () >= Swift::MUCOccupant::Participant)
		{
			Swift::Message::ref msg (new Swift::Message);
			msg->setBody ("Cилы зла стали сильнее.");
			msg->setType (Swift::Message::Groupchat);
			msg->setTo (Swift::JID (room));
			Client_->sendMessage (msg);
		}
	}

	void VortexBot::handleOccupantLeft (const Swift::MUCOccupant& occupant,
			Swift::MUC::LeavingType type, const std::string& str,
			const std::string& room)
	{
		std::cout << occupant.getNick () << " occ left" << std::endl;
		if (occupant.getRole () >= Swift::MUCOccupant::Participant)
		{
			Swift::Message::ref msg (new Swift::Message);
			msg->setBody ("Cилы зла стали cлабее.");
			msg->setType (Swift::Message::Groupchat);
			msg->setTo (Swift::JID (room));
			Client_->sendMessage (msg);
		}
	}

	void VortexBot::handleOccupantRoleChanged (const std::string& str,
			const Swift::MUCOccupant& occupant,
			const Swift::MUCOccupant::Role& role, const std::string& room)
	{
		std::cout << occupant.getNick () << " occ role changed" << std::endl;
		Swift::Message::ref msg (new Swift::Message);
		msg->setType (Swift::Message::Groupchat);
		msg->setTo (Swift::JID (room));
		msg->setBody (occupant.getRole () >= Swift::MUCOccupant::Participant ?
			"Cилы зла стали сильнее." :
			"Cилы зла стали слабее.");
		Client_->sendMessage (msg);
	}

}