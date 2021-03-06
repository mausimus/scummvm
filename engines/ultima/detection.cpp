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

#include "base/plugins.h"

#include "common/translation.h"

#include "ultima/detection.h"
#include "ultima/detection_tables.h"

namespace Ultima {

static const PlainGameDescriptor ULTIMA_GAMES[] = {
#ifndef RELEASE_BUILD
	{ "ultima1", "Ultima I - The First Age of Darkness" },
#endif
	{ "ultima4", "Ultima IV - Quest of the Avatar" },
	{ "ultima4_enh", "Ultima IV - Quest of the Avatar - Enhanced" },
	{ "ultima6", "Ultima VI - The False Prophet" },
	{ "ultima6_enh", "Ultima VI - The False Prophet - Enhanced" },
	{ "ultima8", "Ultima VIII - Pagan" },
	{ "remorse", "Crusader: No Remorse" },
	{ "regret", "Crusader: No Regret" },

	{ "martiandreams", "Worlds of Ultima: Martian Dreams" },
	{ "martiandreams_enh", "Worlds of Ultima: Martian Dreams - Enhanced" },
	{ "thesavageempire", "Worlds of Ultima: The Savage Empire" },
	{ "thesavageempire_enh", "Worlds of Ultima: The Savage Empire - Enhanced" },
	{ 0, 0 }
};

static const ADExtraGuiOptionsMap optionsList[] = {
	{
		GAMEOPTION_ORIGINAL_SAVELOAD,
		{
			_s("Use original save/load screens"),
			_s("Use the original save/load screens instead of the ScummVM ones"),
			"originalsaveload",
			false
		}
	},
    {
		GAMEOPTION_FONT_OVERRIDE,
		{
			_s("Enable font replacement"),
			_s("Replaces game fonts with rendered fonts"),
			"font_override",
			false
		}
	},
	{
		GAMEOPTION_FONT_ANTIALIASING,
		{
			_s("Enable font anti-aliasing"),
			_s("When font anti-aliasing is enabled, the text is smoother."),
			"font_antialiasing",
			false
		}
	},
	{
		GAMEOPTION_FOOTSTEPS,
		{
			_s("Play foot step sounds"),
			_s("Plays sound when the player moves."),
			"footsteps",
			true
		}
	},
	{
		GAMEOPTION_TARGETED_JUMP,
		{
			_s("Enable jump to mouse position"),
			_s("Jumping while not moving targets the mouse cursor rather than direction only."),
			"targetedjump",
			true
		}
	},
	{
		GAMEOPTION_FRAMESKIP,
		{
			_s("Enable frame skipping"),
			_s("Allow the game to skip animation frames when running too slow."),
			"frameSkip",
			false
		}
	},
	{
		GAMEOPTION_FRAMELIMIT,
		{
			_s("Enable frame limiting"),
			_s("Limits the speed of the game to prevent running too fast."),
			"frameLimit",
			true
		}
	},
    AD_EXTRA_GUI_OPTIONS_TERMINATOR
};

} // End of namespace Ultima

UltimaMetaEngineDetection::UltimaMetaEngineDetection() : AdvancedMetaEngineDetection(Ultima::GAME_DESCRIPTIONS,
	        sizeof(Ultima::UltimaGameDescription), Ultima::ULTIMA_GAMES, Ultima::optionsList) {
	static const char *const DIRECTORY_GLOBS[2] = { "usecode", 0 };
	_maxScanDepth = 2;
	_directoryGlobs = DIRECTORY_GLOBS;
}

REGISTER_PLUGIN_STATIC(ULTIMA_DETECTION, PLUGIN_TYPE_ENGINE_DETECTION, UltimaMetaEngineDetection);
