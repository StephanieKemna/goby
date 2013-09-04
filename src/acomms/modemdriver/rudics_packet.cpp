// Copyright 2009-2013 Toby Schneider (https://launchpad.net/~tes)
//                     Massachusetts Institute of Technology (2007-)
//                     Woods Hole Oceanographic Institution (2007-)
//                     Goby Developers Team (https://launchpad.net/~goby-dev)
// 
//
// This file is part of the Goby Underwater Autonomy Project Libraries
// ("The Goby Libraries").
//
// The Goby Libraries are free software: you can redistribute them and/or modify
// them under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// The Goby Libraries are distributed in the hope that they will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with Goby.  If not, see <http://www.gnu.org/licenses/>.

#include <boost/crc.hpp>     
#include <netinet/in.h>
#include <boost/algorithm/string.hpp>

#include "goby/util/base_convert.h"
#include "rudics_packet.h"
#include "goby/util/binary.h"

void goby::acomms::serialize_rudics_packet(std::string bytes, std::string* rudics_pkt)
{
    // append CRC
    boost::crc_32_type crc;
    crc.process_bytes(bytes.data(), bytes.length());
    bytes += uint32_to_byte_string(crc.checksum());

    static const std::string reserved = std::string("\0\r\n",3) +
        std::string(1, 0xff);

    // convert to base (256 minus reserved)
    const int reduced_base = 256-reserved.size();
    std::cout << "new base: " << reduced_base << std::endl;
    
    goby::util::base_convert(bytes, rudics_pkt, 256, reduced_base);

    std::cout << goby::util::hex_encode(*rudics_pkt) << std::endl;

    
    for(int i = 0, n = reserved.size(); i < n; ++i)
    {
        std::replace(rudics_pkt->begin(),
                     rudics_pkt->end(),
                     reserved[i],
                     static_cast<char>(reduced_base+i));
        std::cout << goby::util::hex_encode(*rudics_pkt) << std::endl;
    }
    
//    *rudics_pkt = goby::util::hex_encode(bytes);
    
    *rudics_pkt += "\r";
}

void goby::acomms::parse_rudics_packet(std::string* bytes, std::string rudics_pkt)
{    
    const unsigned CR_SIZE = 1;    
    if(rudics_pkt.size() < CR_SIZE)
        throw(RudicsPacketException("Packet too short for <CR>"));

    rudics_pkt = rudics_pkt.substr(0, rudics_pkt.size()-1);


    static const std::string reserved = std::string("\0\r\n", 3) +
        std::string(1, 0xff);
    const int reduced_base = 256-reserved.size();

    // get rid of extra junk
    std::remove_if(rudics_pkt.begin(), rudics_pkt.end(), boost::is_any_of(reserved));

    for(int i = 0, n = reserved.size(); i < n; ++i)
    {    
        std::replace(rudics_pkt.begin(),
                     rudics_pkt.end(),
                     static_cast<char>(reduced_base+i),
                     reserved[i]);
    }
    
    goby::util::base_convert(rudics_pkt, bytes, reduced_base, 256);

//    *bytes = goby::util::hex_decode(rudics_pkt);

    const unsigned CRC_BYTE_SIZE = 4;
    if(bytes->size() < CRC_BYTE_SIZE)
        throw(RudicsPacketException("Packet too short for CRC32"));

    std::string crc_str = bytes->substr(bytes->size()-4, 4);
    uint32_t given_crc = byte_string_to_uint32(crc_str);
    *bytes = bytes->substr(0, bytes->size()-4);

    boost::crc_32_type crc;
    crc.process_bytes(bytes->data(), bytes->length());
    uint32_t computed_crc = crc.checksum();

    if(given_crc != computed_crc)
        throw(RudicsPacketException("Bad CRC32"));
    
}

std::string goby::acomms::uint32_to_byte_string(uint32_t i)
{
    union u_t { uint32_t i; char c[4]; } u;
    u.i = htonl(i);
    return std::string(u.c, 4);
}

uint32_t goby::acomms::byte_string_to_uint32(std::string s)
{
    union u_t { uint32_t i; char c[4]; } u;
    memcpy(u.c, s.c_str(), 4);
    return ntohl(u.i);
}
