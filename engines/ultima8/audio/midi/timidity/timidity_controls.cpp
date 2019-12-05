/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#include "ultima8/misc/pent_include.h"

#ifdef USE_TIMIDITY_MIDI

#include "timidity.h"
#include "timidity_controls.h"

namespace Ultima8 {

#ifdef NS_TIMIDITY
namespace NS_TIMIDITY {
#endif

#ifdef SDL
extern ControlMode sdl_control_mode;
# ifndef DEFAULT_CONTROL_MODE
#  define DEFAULT_CONTROL_MODE &sdl_control_mode
# endif
#endif

ControlMode *ctl_list[] = {
#ifdef SDL
	&sdl_control_mode,
#endif
	0
};

ControlMode *ctl = DEFAULT_CONTROL_MODE;

#ifdef NS_TIMIDITY
};
#endif

} // End of namespace Ultima8

#endif