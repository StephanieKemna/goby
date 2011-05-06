// copyright 2011 t. schneider tes@mit.edu
// 
// this file is part of goby-acomms, a collection of libraries for acoustic underwater networking
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This software is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this software.  If not, see <http://www.gnu.org/licenses/>.

#ifndef AcommsHelpers20110208H
#define AcommsHelpers20110208H

#include <string>
#include <limits>
#include <bitset>

#include "goby/protobuf/modem_message.pb.h"
#include "goby/core/core_constants.h"

namespace goby
{

    namespace acomms
    {            
        // provides stream output operator for Google Protocol Buffers Message 
        inline std::ostream& operator<<(std::ostream& out, const google::protobuf::Message& msg)
        {
            return (out << "[[" << msg.GetDescriptor()->name() <<"]] " << msg.ShortDebugString());
        }


        class ManipulatorManager
        {
          public:
            void add(unsigned id, goby::acomms::protobuf::MessageFile::Manipulator manip)
            {
                manips_.insert(std::make_pair(id, manip));
            }            
            
            bool has(unsigned id, goby::acomms::protobuf::MessageFile::Manipulator manip) const
            {
                typedef std::multimap<unsigned, goby::acomms::protobuf::MessageFile::Manipulator>::const_iterator iterator;
                std::pair<iterator,iterator> p = manips_.equal_range(id);

                for(iterator it = p.first; it != p.second; ++it)
                {
                    if(it->second == manip)
                        return true;
                }

                return false;
            }

            void clear()
            {
                manips_.clear();
            }
            
          private:
            // manipulator multimap (no_encode, no_decode, etc)
            // maps DCCL ID (unsigned) onto Manipulator enumeration (xml_config.proto)
            std::multimap<unsigned, goby::acomms::protobuf::MessageFile::Manipulator> manips_;

        };
        
        

        
    }
}

#endif
