// ndppd - NDP Proxy Daemon
// Copyright (C) 2011-2018  Daniel Adolfsson <daniel@priv.nu>
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

#include <string>
#include <arpa/inet.h>
#include <vector>
#include <stdexcept>
#include <cassert>
#include <cstring>
#include <iostream>
#include <algorithm>

#include "ndppd.h"
#include "cidr.h"

NDPPD_NS_BEGIN

namespace {

uint32_t kMaskTable[][4] {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
        { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
        { 0x00000080, 0x00000000, 0x00000000, 0x00000000 },
        { 0x000000c0, 0x00000000, 0x00000000, 0x00000000 },
        { 0x000000e0, 0x00000000, 0x00000000, 0x00000000 },
        { 0x000000f0, 0x00000000, 0x00000000, 0x00000000 },
        { 0x000000f8, 0x00000000, 0x00000000, 0x00000000 },
        { 0x000000fc, 0x00000000, 0x00000000, 0x00000000 },
        { 0x000000fe, 0x00000000, 0x00000000, 0x00000000 },
        { 0x000000ff, 0x00000000, 0x00000000, 0x00000000 },
        { 0x000080ff, 0x00000000, 0x00000000, 0x00000000 },
        { 0x0000c0ff, 0x00000000, 0x00000000, 0x00000000 },
        { 0x0000e0ff, 0x00000000, 0x00000000, 0x00000000 },
        { 0x0000f0ff, 0x00000000, 0x00000000, 0x00000000 },
        { 0x0000f8ff, 0x00000000, 0x00000000, 0x00000000 },
        { 0x0000fcff, 0x00000000, 0x00000000, 0x00000000 },
        { 0x0000feff, 0x00000000, 0x00000000, 0x00000000 },
        { 0x0000ffff, 0x00000000, 0x00000000, 0x00000000 },
        { 0x0080ffff, 0x00000000, 0x00000000, 0x00000000 },
        { 0x00c0ffff, 0x00000000, 0x00000000, 0x00000000 },
        { 0x00e0ffff, 0x00000000, 0x00000000, 0x00000000 },
        { 0x00f0ffff, 0x00000000, 0x00000000, 0x00000000 },
        { 0x00f8ffff, 0x00000000, 0x00000000, 0x00000000 },
        { 0x00fcffff, 0x00000000, 0x00000000, 0x00000000 },
        { 0x00feffff, 0x00000000, 0x00000000, 0x00000000 },
        { 0x00ffffff, 0x00000000, 0x00000000, 0x00000000 },
        { 0x80ffffff, 0x00000000, 0x00000000, 0x00000000 },
        { 0xc0ffffff, 0x00000000, 0x00000000, 0x00000000 },
        { 0xe0ffffff, 0x00000000, 0x00000000, 0x00000000 },
        { 0xf0ffffff, 0x00000000, 0x00000000, 0x00000000 },
        { 0xf8ffffff, 0x00000000, 0x00000000, 0x00000000 },
        { 0xfcffffff, 0x00000000, 0x00000000, 0x00000000 },
        { 0xfeffffff, 0x00000000, 0x00000000, 0x00000000 },
        { 0xffffffff, 0x00000000, 0x00000000, 0x00000000 },
        { 0xffffffff, 0x00000080, 0x00000000, 0x00000000 },
        { 0xffffffff, 0x000000c0, 0x00000000, 0x00000000 },
        { 0xffffffff, 0x000000e0, 0x00000000, 0x00000000 },
        { 0xffffffff, 0x000000f0, 0x00000000, 0x00000000 },
        { 0xffffffff, 0x000000f8, 0x00000000, 0x00000000 },
        { 0xffffffff, 0x000000fc, 0x00000000, 0x00000000 },
        { 0xffffffff, 0x000000fe, 0x00000000, 0x00000000 },
        { 0xffffffff, 0x000000ff, 0x00000000, 0x00000000 },
        { 0xffffffff, 0x000080ff, 0x00000000, 0x00000000 },
        { 0xffffffff, 0x0000c0ff, 0x00000000, 0x00000000 },
        { 0xffffffff, 0x0000e0ff, 0x00000000, 0x00000000 },
        { 0xffffffff, 0x0000f0ff, 0x00000000, 0x00000000 },
        { 0xffffffff, 0x0000f8ff, 0x00000000, 0x00000000 },
        { 0xffffffff, 0x0000fcff, 0x00000000, 0x00000000 },
        { 0xffffffff, 0x0000feff, 0x00000000, 0x00000000 },
        { 0xffffffff, 0x0000ffff, 0x00000000, 0x00000000 },
        { 0xffffffff, 0x0080ffff, 0x00000000, 0x00000000 },
        { 0xffffffff, 0x00c0ffff, 0x00000000, 0x00000000 },
        { 0xffffffff, 0x00e0ffff, 0x00000000, 0x00000000 },
        { 0xffffffff, 0x00f0ffff, 0x00000000, 0x00000000 },
        { 0xffffffff, 0x00f8ffff, 0x00000000, 0x00000000 },
        { 0xffffffff, 0x00fcffff, 0x00000000, 0x00000000 },
        { 0xffffffff, 0x00feffff, 0x00000000, 0x00000000 },
        { 0xffffffff, 0x00ffffff, 0x00000000, 0x00000000 },
        { 0xffffffff, 0x80ffffff, 0x00000000, 0x00000000 },
        { 0xffffffff, 0xc0ffffff, 0x00000000, 0x00000000 },
        { 0xffffffff, 0xe0ffffff, 0x00000000, 0x00000000 },
        { 0xffffffff, 0xf0ffffff, 0x00000000, 0x00000000 },
        { 0xffffffff, 0xf8ffffff, 0x00000000, 0x00000000 },
        { 0xffffffff, 0xfcffffff, 0x00000000, 0x00000000 },
        { 0xffffffff, 0xfeffffff, 0x00000000, 0x00000000 },
        { 0xffffffff, 0xffffffff, 0x00000000, 0x00000000 },
        { 0xffffffff, 0xffffffff, 0x00000080, 0x00000000 },
        { 0xffffffff, 0xffffffff, 0x000000c0, 0x00000000 },
        { 0xffffffff, 0xffffffff, 0x000000e0, 0x00000000 },
        { 0xffffffff, 0xffffffff, 0x000000f0, 0x00000000 },
        { 0xffffffff, 0xffffffff, 0x000000f8, 0x00000000 },
        { 0xffffffff, 0xffffffff, 0x000000fc, 0x00000000 },
        { 0xffffffff, 0xffffffff, 0x000000fe, 0x00000000 },
        { 0xffffffff, 0xffffffff, 0x000000ff, 0x00000000 },
        { 0xffffffff, 0xffffffff, 0x000080ff, 0x00000000 },
        { 0xffffffff, 0xffffffff, 0x0000c0ff, 0x00000000 },
        { 0xffffffff, 0xffffffff, 0x0000e0ff, 0x00000000 },
        { 0xffffffff, 0xffffffff, 0x0000f0ff, 0x00000000 },
        { 0xffffffff, 0xffffffff, 0x0000f8ff, 0x00000000 },
        { 0xffffffff, 0xffffffff, 0x0000fcff, 0x00000000 },
        { 0xffffffff, 0xffffffff, 0x0000feff, 0x00000000 },
        { 0xffffffff, 0xffffffff, 0x0000ffff, 0x00000000 },
        { 0xffffffff, 0xffffffff, 0x0080ffff, 0x00000000 },
        { 0xffffffff, 0xffffffff, 0x00c0ffff, 0x00000000 },
        { 0xffffffff, 0xffffffff, 0x00e0ffff, 0x00000000 },
        { 0xffffffff, 0xffffffff, 0x00f0ffff, 0x00000000 },
        { 0xffffffff, 0xffffffff, 0x00f8ffff, 0x00000000 },
        { 0xffffffff, 0xffffffff, 0x00fcffff, 0x00000000 },
        { 0xffffffff, 0xffffffff, 0x00feffff, 0x00000000 },
        { 0xffffffff, 0xffffffff, 0x00ffffff, 0x00000000 },
        { 0xffffffff, 0xffffffff, 0x80ffffff, 0x00000000 },
        { 0xffffffff, 0xffffffff, 0xc0ffffff, 0x00000000 },
        { 0xffffffff, 0xffffffff, 0xe0ffffff, 0x00000000 },
        { 0xffffffff, 0xffffffff, 0xf0ffffff, 0x00000000 },
        { 0xffffffff, 0xffffffff, 0xf8ffffff, 0x00000000 },
        { 0xffffffff, 0xffffffff, 0xfcffffff, 0x00000000 },
        { 0xffffffff, 0xffffffff, 0xfeffffff, 0x00000000 },
        { 0xffffffff, 0xffffffff, 0xffffffff, 0x00000000 },
        { 0xffffffff, 0xffffffff, 0xffffffff, 0x00000080 },
        { 0xffffffff, 0xffffffff, 0xffffffff, 0x000000c0 },
        { 0xffffffff, 0xffffffff, 0xffffffff, 0x000000e0 },
        { 0xffffffff, 0xffffffff, 0xffffffff, 0x000000f0 },
        { 0xffffffff, 0xffffffff, 0xffffffff, 0x000000f8 },
        { 0xffffffff, 0xffffffff, 0xffffffff, 0x000000fc },
        { 0xffffffff, 0xffffffff, 0xffffffff, 0x000000fe },
        { 0xffffffff, 0xffffffff, 0xffffffff, 0x000000ff },
        { 0xffffffff, 0xffffffff, 0xffffffff, 0x000080ff },
        { 0xffffffff, 0xffffffff, 0xffffffff, 0x0000c0ff },
        { 0xffffffff, 0xffffffff, 0xffffffff, 0x0000e0ff },
        { 0xffffffff, 0xffffffff, 0xffffffff, 0x0000f0ff },
        { 0xffffffff, 0xffffffff, 0xffffffff, 0x0000f8ff },
        { 0xffffffff, 0xffffffff, 0xffffffff, 0x0000fcff },
        { 0xffffffff, 0xffffffff, 0xffffffff, 0x0000feff },
        { 0xffffffff, 0xffffffff, 0xffffffff, 0x0000ffff },
        { 0xffffffff, 0xffffffff, 0xffffffff, 0x0080ffff },
        { 0xffffffff, 0xffffffff, 0xffffffff, 0x00c0ffff },
        { 0xffffffff, 0xffffffff, 0xffffffff, 0x00e0ffff },
        { 0xffffffff, 0xffffffff, 0xffffffff, 0x00f0ffff },
        { 0xffffffff, 0xffffffff, 0xffffffff, 0x00f8ffff },
        { 0xffffffff, 0xffffffff, 0xffffffff, 0x00fcffff },
        { 0xffffffff, 0xffffffff, 0xffffffff, 0x00feffff },
        { 0xffffffff, 0xffffffff, 0xffffffff, 0x00ffffff },
        { 0xffffffff, 0xffffffff, 0xffffffff, 0x80ffffff },
        { 0xffffffff, 0xffffffff, 0xffffffff, 0xc0ffffff },
        { 0xffffffff, 0xffffffff, 0xffffffff, 0xe0ffffff },
        { 0xffffffff, 0xffffffff, 0xffffffff, 0xf0ffffff },
        { 0xffffffff, 0xffffffff, 0xffffffff, 0xf8ffffff },
        { 0xffffffff, 0xffffffff, 0xffffffff, 0xfcffffff },
        { 0xffffffff, 0xffffffff, 0xffffffff, 0xfeffffff },
        { 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff }
#else
    { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
    { 0x80000000, 0x00000000, 0x00000000, 0x00000000 },
    { 0xc0000000, 0x00000000, 0x00000000, 0x00000000 },
    { 0xe0000000, 0x00000000, 0x00000000, 0x00000000 },
    { 0xf0000000, 0x00000000, 0x00000000, 0x00000000 },
    { 0xf8000000, 0x00000000, 0x00000000, 0x00000000 },
    { 0xfc000000, 0x00000000, 0x00000000, 0x00000000 },
    { 0xfe000000, 0x00000000, 0x00000000, 0x00000000 },
    { 0xff000000, 0x00000000, 0x00000000, 0x00000000 },
    { 0xff800000, 0x00000000, 0x00000000, 0x00000000 },
    { 0xffc00000, 0x00000000, 0x00000000, 0x00000000 },
    { 0xffe00000, 0x00000000, 0x00000000, 0x00000000 },
    { 0xfff00000, 0x00000000, 0x00000000, 0x00000000 },
    { 0xfff80000, 0x00000000, 0x00000000, 0x00000000 },
    { 0xfffc0000, 0x00000000, 0x00000000, 0x00000000 },
    { 0xfffe0000, 0x00000000, 0x00000000, 0x00000000 },
    { 0xffff0000, 0x00000000, 0x00000000, 0x00000000 },
    { 0xffff8000, 0x00000000, 0x00000000, 0x00000000 },
    { 0xffffc000, 0x00000000, 0x00000000, 0x00000000 },
    { 0xffffe000, 0x00000000, 0x00000000, 0x00000000 },
    { 0xfffff000, 0x00000000, 0x00000000, 0x00000000 },
    { 0xfffff800, 0x00000000, 0x00000000, 0x00000000 },
    { 0xfffffc00, 0x00000000, 0x00000000, 0x00000000 },
    { 0xfffffe00, 0x00000000, 0x00000000, 0x00000000 },
    { 0xffffff00, 0x00000000, 0x00000000, 0x00000000 },
    { 0xffffff80, 0x00000000, 0x00000000, 0x00000000 },
    { 0xffffffc0, 0x00000000, 0x00000000, 0x00000000 },
    { 0xffffffe0, 0x00000000, 0x00000000, 0x00000000 },
    { 0xfffffff0, 0x00000000, 0x00000000, 0x00000000 },
    { 0xfffffff8, 0x00000000, 0x00000000, 0x00000000 },
    { 0xfffffffc, 0x00000000, 0x00000000, 0x00000000 },
    { 0xfffffffe, 0x00000000, 0x00000000, 0x00000000 },
    { 0xffffffff, 0x00000000, 0x00000000, 0x00000000 },
    { 0xffffffff, 0x80000000, 0x00000000, 0x00000000 },
    { 0xffffffff, 0xc0000000, 0x00000000, 0x00000000 },
    { 0xffffffff, 0xe0000000, 0x00000000, 0x00000000 },
    { 0xffffffff, 0xf0000000, 0x00000000, 0x00000000 },
    { 0xffffffff, 0xf8000000, 0x00000000, 0x00000000 },
    { 0xffffffff, 0xfc000000, 0x00000000, 0x00000000 },
    { 0xffffffff, 0xfe000000, 0x00000000, 0x00000000 },
    { 0xffffffff, 0xff000000, 0x00000000, 0x00000000 },
    { 0xffffffff, 0xff800000, 0x00000000, 0x00000000 },
    { 0xffffffff, 0xffc00000, 0x00000000, 0x00000000 },
    { 0xffffffff, 0xffe00000, 0x00000000, 0x00000000 },
    { 0xffffffff, 0xfff00000, 0x00000000, 0x00000000 },
    { 0xffffffff, 0xfff80000, 0x00000000, 0x00000000 },
    { 0xffffffff, 0xfffc0000, 0x00000000, 0x00000000 },
    { 0xffffffff, 0xfffe0000, 0x00000000, 0x00000000 },
    { 0xffffffff, 0xffff0000, 0x00000000, 0x00000000 },
    { 0xffffffff, 0xffff8000, 0x00000000, 0x00000000 },
    { 0xffffffff, 0xffffc000, 0x00000000, 0x00000000 },
    { 0xffffffff, 0xffffe000, 0x00000000, 0x00000000 },
    { 0xffffffff, 0xfffff000, 0x00000000, 0x00000000 },
    { 0xffffffff, 0xfffff800, 0x00000000, 0x00000000 },
    { 0xffffffff, 0xfffffc00, 0x00000000, 0x00000000 },
    { 0xffffffff, 0xfffffe00, 0x00000000, 0x00000000 },
    { 0xffffffff, 0xffffff00, 0x00000000, 0x00000000 },
    { 0xffffffff, 0xffffff80, 0x00000000, 0x00000000 },
    { 0xffffffff, 0xffffffc0, 0x00000000, 0x00000000 },
    { 0xffffffff, 0xffffffe0, 0x00000000, 0x00000000 },
    { 0xffffffff, 0xfffffff0, 0x00000000, 0x00000000 },
    { 0xffffffff, 0xfffffff8, 0x00000000, 0x00000000 },
    { 0xffffffff, 0xfffffffc, 0x00000000, 0x00000000 },
    { 0xffffffff, 0xfffffffe, 0x00000000, 0x00000000 },
    { 0xffffffff, 0xffffffff, 0x00000000, 0x00000000 },
    { 0xffffffff, 0xffffffff, 0x80000000, 0x00000000 },
    { 0xffffffff, 0xffffffff, 0xc0000000, 0x00000000 },
    { 0xffffffff, 0xffffffff, 0xe0000000, 0x00000000 },
    { 0xffffffff, 0xffffffff, 0xf0000000, 0x00000000 },
    { 0xffffffff, 0xffffffff, 0xf8000000, 0x00000000 },
    { 0xffffffff, 0xffffffff, 0xfc000000, 0x00000000 },
    { 0xffffffff, 0xffffffff, 0xfe000000, 0x00000000 },
    { 0xffffffff, 0xffffffff, 0xff000000, 0x00000000 },
    { 0xffffffff, 0xffffffff, 0xff800000, 0x00000000 },
    { 0xffffffff, 0xffffffff, 0xffc00000, 0x00000000 },
    { 0xffffffff, 0xffffffff, 0xffe00000, 0x00000000 },
    { 0xffffffff, 0xffffffff, 0xfff00000, 0x00000000 },
    { 0xffffffff, 0xffffffff, 0xfff80000, 0x00000000 },
    { 0xffffffff, 0xffffffff, 0xfffc0000, 0x00000000 },
    { 0xffffffff, 0xffffffff, 0xfffe0000, 0x00000000 },
    { 0xffffffff, 0xffffffff, 0xffff0000, 0x00000000 },
    { 0xffffffff, 0xffffffff, 0xffff8000, 0x00000000 },
    { 0xffffffff, 0xffffffff, 0xffffc000, 0x00000000 },
    { 0xffffffff, 0xffffffff, 0xffffe000, 0x00000000 },
    { 0xffffffff, 0xffffffff, 0xfffff000, 0x00000000 },
    { 0xffffffff, 0xffffffff, 0xfffff800, 0x00000000 },
    { 0xffffffff, 0xffffffff, 0xfffffc00, 0x00000000 },
    { 0xffffffff, 0xffffffff, 0xfffffe00, 0x00000000 },
    { 0xffffffff, 0xffffffff, 0xffffff00, 0x00000000 },
    { 0xffffffff, 0xffffffff, 0xffffff80, 0x00000000 },
    { 0xffffffff, 0xffffffff, 0xffffffc0, 0x00000000 },
    { 0xffffffff, 0xffffffff, 0xffffffe0, 0x00000000 },
    { 0xffffffff, 0xffffffff, 0xfffffff0, 0x00000000 },
    { 0xffffffff, 0xffffffff, 0xfffffff8, 0x00000000 },
    { 0xffffffff, 0xffffffff, 0xfffffffc, 0x00000000 },
    { 0xffffffff, 0xffffffff, 0xfffffffe, 0x00000000 },
    { 0xffffffff, 0xffffffff, 0xffffffff, 0x00000000 },
    { 0xffffffff, 0xffffffff, 0xffffffff, 0x80000000 },
    { 0xffffffff, 0xffffffff, 0xffffffff, 0xc0000000 },
    { 0xffffffff, 0xffffffff, 0xffffffff, 0xe0000000 },
    { 0xffffffff, 0xffffffff, 0xffffffff, 0xf0000000 },
    { 0xffffffff, 0xffffffff, 0xffffffff, 0xf8000000 },
    { 0xffffffff, 0xffffffff, 0xffffffff, 0xfc000000 },
    { 0xffffffff, 0xffffffff, 0xffffffff, 0xfe000000 },
    { 0xffffffff, 0xffffffff, 0xffffffff, 0xff000000 },
    { 0xffffffff, 0xffffffff, 0xffffffff, 0xff800000 },
    { 0xffffffff, 0xffffffff, 0xffffffff, 0xffc00000 },
    { 0xffffffff, 0xffffffff, 0xffffffff, 0xffe00000 },
    { 0xffffffff, 0xffffffff, 0xffffffff, 0xfff00000 },
    { 0xffffffff, 0xffffffff, 0xffffffff, 0xfff80000 },
    { 0xffffffff, 0xffffffff, 0xffffffff, 0xfffc0000 },
    { 0xffffffff, 0xffffffff, 0xffffffff, 0xfffe0000 },
    { 0xffffffff, 0xffffffff, 0xffffffff, 0xffff0000 },
    { 0xffffffff, 0xffffffff, 0xffffffff, 0xffff8000 },
    { 0xffffffff, 0xffffffff, 0xffffffff, 0xffffc000 },
    { 0xffffffff, 0xffffffff, 0xffffffff, 0xffffe000 },
    { 0xffffffff, 0xffffffff, 0xffffffff, 0xfffff000 },
    { 0xffffffff, 0xffffffff, 0xffffffff, 0xfffff800 },
    { 0xffffffff, 0xffffffff, 0xffffffff, 0xfffffc00 },
    { 0xffffffff, 0xffffffff, 0xffffffff, 0xfffffe00 },
    { 0xffffffff, 0xffffffff, 0xffffffff, 0xffffff00 },
    { 0xffffffff, 0xffffffff, 0xffffffff, 0xffffff80 },
    { 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffc0 },
    { 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffe0 },
    { 0xffffffff, 0xffffffff, 0xffffffff, 0xfffffff0 },
    { 0xffffffff, 0xffffffff, 0xffffffff, 0xfffffff8 },
    { 0xffffffff, 0xffffffff, 0xffffffff, 0xfffffffc },
    { 0xffffffff, 0xffffffff, 0xffffffff, 0xfffffffe },
    { 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff }
#endif
};

}

Cidr::Cidr()
        : addr {}, _prefix(128)
{
}

Cidr::Cidr(std::string string)
{
    auto offset = string.find('/');

    if (offset != std::string::npos) {
        string[offset] = 0;
        _prefix = std::stoi(&string[offset + 1]);
    }
    else
        _prefix = 128;

    if (::inet_pton(AF_INET6, string.c_str(), &addr) <= 0)
        throw std::runtime_error("Failed to parse address part of CIDR");
}

Cidr::Cidr(const in6_addr& addr, int prefix)
        : addr(addr)
{
    this->prefix(prefix);
}

Cidr::operator bool() const
{
    return !(addr.s6_addr32[0] == 0 &&
             addr.s6_addr32[1] == 0 &&
             addr.s6_addr32[2] == 0 &&
             addr.s6_addr32[3] == 0 &&
             _prefix == 0);
}

const std::string Cidr::to_string() const
{
    char buf[INET6_ADDRSTRLEN + 8];
    assert(::inet_ntop(AF_INET6, &addr, buf, INET6_ADDRSTRLEN) != nullptr);

    if (_prefix < 128)
        sprintf(buf + ::strlen(buf), "/%d", _prefix);

    return buf;
}

bool Cidr::operator%(const Address& address) const
{
    const auto& mask = kMaskTable[_prefix];
    return !(((address.addr.s6_addr32[0] ^ addr.s6_addr32[0]) & mask[0]) |
             ((address.addr.s6_addr32[1] ^ addr.s6_addr32[1]) & mask[1]) |
             ((address.addr.s6_addr32[2] ^ addr.s6_addr32[2]) & mask[2]) |
             ((address.addr.s6_addr32[3] ^ addr.s6_addr32[3]) & mask[3]));
}

int Cidr::prefix() const
{
    return _prefix;
}

void Cidr::prefix(int prefix)
{
    _prefix = prefix < 0 ? 0 : prefix > 128 ? 128 : prefix;
}

Logger& operator<<(Logger& logger, const Cidr& cidr)
{
    logger << cidr.to_string();
    return logger;
}

NDPPD_NS_END
