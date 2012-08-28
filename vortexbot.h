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


#pragma once

#include <string>
#include <boost/program_options.hpp>
#include <Swiften/Client/Client.h>
#include <Swiften/Elements/Message.h>
#include <Swiften/MUC/MUC.h>

namespace XmppBot
{
	class VortexBot
	{
		boost::program_options::variables_map VM_;
		Swift::Client *Client_;
		std::map<std::string, Swift::MUC::ref> JoinedMucs_;
	public:
		VortexBot (const std::string& configFile);
		~VortexBot ();

		void Connect ();

	private:
		void onConnected ();
		void onMucJoin (const std::string& room, const std::string& str);
		void handleOccupantJoin (const Swift::MUCOccupant& occupant,
				const std::string& room);
		void handleOccupantLeft (const Swift::MUCOccupant& occupant,
				Swift::MUC::LeavingType type, const std::string& str,
				const std::string& room);
		void handleOccupantRoleChanged (const std::string& str,
				const Swift::MUCOccupant& occupant,
				const Swift::MUCOccupant::Role& role,
				const std::string& room);

	};
}
