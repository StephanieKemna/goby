// copyright 2011 t. schneider tes@mit.edu
// 
// this file is a collection of time conversion utilities used in goby
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

#include "goby/util/time.h"

double goby::util::ptime2unix_double(boost::posix_time::ptime given_time)
{
    using namespace boost::posix_time;
    using namespace boost::gregorian;
        
    if (given_time == not_a_date_time)
        return -1;
    else
    {
        time_duration diff = given_time - ptime(date(1970,1,1));
        return static_cast<double>(diff.total_seconds()) +
            static_cast<double>(diff.fractional_seconds()) /
            static_cast<double>(time_duration::ticks_per_second());
    }
}

boost::posix_time::ptime goby::util::unix_double2ptime(double given_time)
{
    using namespace boost::posix_time;
    using namespace boost::gregorian;
    
    if (given_time == -1)
        return boost::posix_time::ptime(not_a_date_time);
    else
    {
        ptime time_t_epoch(date(1970,1,1));
        long s = floor(given_time);
        long micro_s = (given_time - s)*1e6;
        return time_t_epoch + seconds(s) + microseconds(micro_s);
    }
}

goby::uint64 goby::util::ptime2unix_microsec(boost::posix_time::ptime given_time)
{
    using namespace boost::posix_time;
    using namespace boost::gregorian;
        
    if (given_time == not_a_date_time)
        return std::numeric_limits<uint64>::max();
    else
    {
        const int MICROSEC_IN_SEC = 1000000;
        time_duration diff = given_time - ptime(date(1970,1,1));
        return static_cast<uint64>(diff.total_seconds())*MICROSEC_IN_SEC +
            static_cast<uint64>(diff.fractional_seconds()) /
            (time_duration::ticks_per_second() / MICROSEC_IN_SEC);
    }    
}

    
/// convert to boost date_time ptime from the number of microseconds since 1/1/1970 0:00 UTC ("UNIX Time"): good to the microsecond
boost::posix_time::ptime goby::util::unix_microsec2ptime(uint64 given_time)
{
    using namespace boost::posix_time;
    using namespace boost::gregorian;
    
    if (given_time == std::numeric_limits<uint64>::max())
        return boost::posix_time::ptime(not_a_date_time);
    else
    {
        const int MICROSEC_IN_SEC = 1000000;
        ptime time_t_epoch(date(1970,1,1));
        uint64 s = given_time / MICROSEC_IN_SEC;
        uint64 micro_s = (given_time - s * MICROSEC_IN_SEC);
        return time_t_epoch + seconds(s) + microseconds(micro_s);
    }
}
