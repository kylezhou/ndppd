// ndppd - NDP Proxy Daemon
// Copyright (C) 2011  Daniel Adolfsson <daniel@priv.nu>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "nl_address.h"
#include "address.h"

NDPPD_NS_BEGIN

NetlinkAddress::NetlinkAddress(const ndppd::Address& address, const std::shared_ptr<Interface>& iface)
        : _address(address), _iface(iface)
{
}

const Address& NetlinkAddress::address() const
{
    return _address;
}

const std::shared_ptr<Interface>& NetlinkAddress::iface() const
{
    return _iface;
}

bool NetlinkAddress::operator<(const NetlinkAddress& rval) const
{
    return _address < rval._address || (_address == rval._address && _iface->index() < rval._iface->index());
}

NDPPD_NS_END
