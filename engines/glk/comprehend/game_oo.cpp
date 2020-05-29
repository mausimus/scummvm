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

#include "glk/comprehend/comprehend.h"
#include "glk/comprehend/game_data.h"
#include "glk/comprehend/game.h"
#include "glk/comprehend/graphics.h"

namespace Glk {
namespace Comprehend {

#define OO_ROOM_FLAG_DARK	0x02

#define OO_BRIGHT_ROOM		0x19

#define OO_FLAG_WEARING_GOGGLES	0x1b
#define OO_FLAG_FLASHLIGHT_ON	0x27

static int oo_room_is_special(struct comprehend_game *game, 
			      unsigned room_index,
			      unsigned *room_desc_string)
{
	struct room *room = &game->info->rooms[room_index];

	/* Is the room dark */
	if ((room->flags & OO_ROOM_FLAG_DARK) &&
	    !(game->info->flags[OO_FLAG_FLASHLIGHT_ON])) {
		if (room_desc_string)
			*room_desc_string = 0xb3; 
		return ROOM_IS_DARK;
	}

	/* Is the room too bright */
	if (room_index == OO_BRIGHT_ROOM && 
	    !game->info->flags[OO_FLAG_WEARING_GOGGLES]) {
		if (room_desc_string)
			*room_desc_string = 0x1c;
		return ROOM_IS_TOO_BRIGHT;
	}

	return ROOM_IS_NORMAL;
}

static bool oo_before_turn(struct comprehend_game *game)
{
	/* FIXME - probably doesn't work correctly with restored games */
	static bool flashlight_was_on = false, googles_were_worn = false;
	struct room *room = &game->info->rooms[game->info->current_room];

	/* 
	 * Check if the room needs to be redrawn because the flashlight
	 * was switch off or on.
	 */
	if (game->info->flags[OO_FLAG_FLASHLIGHT_ON] != flashlight_was_on &&
	    (room->flags & OO_ROOM_FLAG_DARK)) {
		flashlight_was_on = game->info->flags[OO_FLAG_FLASHLIGHT_ON];
		game->info->update_flags |= UPDATE_GRAPHICS | UPDATE_ROOM_DESC;
	}

	/*
	 * Check if the room needs to be redrawn because the goggles were
	 * put on or removed.
	 */
	if (game->info->flags[OO_FLAG_WEARING_GOGGLES] != googles_were_worn &&
	    game->info->current_room == OO_BRIGHT_ROOM) {
		googles_were_worn = game->info->flags[OO_FLAG_WEARING_GOGGLES];
		game->info->update_flags |= UPDATE_GRAPHICS | UPDATE_ROOM_DESC;
	}

	return false;
}

static void oo_handle_special_opcode(struct comprehend_game *game,
				     uint8 operand)
{
	switch (operand) {
	case 0x03:
		/* Game over - failure */
	case 0x05:
		/* Won the game */
	case 0x04:
		/* Restart game */
		game_restart(game);
		break;

	case 0x06:
		/* Save game */
		game_save(game);
		break;

	case 0x07:
		/* Restore game */
		game_restore(game);
		break;
	}
}

static struct game_ops oo_ops = {
	nullptr,
	nullptr,
	oo_before_turn,
	nullptr,
	oo_room_is_special,
	oo_handle_special_opcode
}; 

struct comprehend_game game_oo_topos = {
	"Oo-Topos",
	"oo",
	"G0",
	{
		// Extra strings are (annoyingly) stored in the game binary
		{"NOVEL.EXE", 0x16564, 0x17640},
		{"NOVEL.EXE", 0x17702, 0x18600},
		{"NOVEL.EXE", 0x186b2, 0x19b80},
		{"NOVEL.EXE", 0x19c62, 0x1a590},
		{"NOVEL.EXE", 0x1a634, 0x1b080},
	},
	{"RA", "RB", "RC", "RD", "RE"},
	{"OA", "OB", "OC", "OD"},
	"G%d",
	1,
	nullptr,
	&oo_ops,
	nullptr
};

} // namespace Comprehend
} // namespace Glk