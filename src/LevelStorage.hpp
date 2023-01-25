#pragma once

#include <string>

namespace tulip::dependencies {
	using geode::Result;
	class Level;

	class LevelStorage {
	public:
		static void addKicking();
		static void removeKicking();
		static bool hasKicking();

		static void storePermanent(std::string const& data);
		static std::string retrievePermanent();

		static Level* getLevel();
	};
}