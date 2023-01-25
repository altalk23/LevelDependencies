#pragma once

#include "Macros.hpp"

#include <Geode/external/json/json_fwd.hpp>
#include <Geode/utils/Result.hpp>
#include <memory>
#include <string>
#include <vector>

class GJGameLevel;
class PlayLayer;
class LevelEditorLayer;

namespace geode {
	class Mod;
}

namespace tulip::dependencies {
	using geode::Result;

	class LevelImpl;
	class LevelPool;

	class Level final {
		class Impl;
		std::unique_ptr<Impl> m_impl;

	public:
		Level() noexcept;
		~Level() noexcept;

		Level(Level const&) = delete;
		Level(Level&&) = delete;
		Level& operator=(Level const&) = delete;
		Level& operator=(Level&&) = delete;

		// Needs a valid GJBaseGameLayer instance
		static Result<Level*> get() noexcept;

		Result<> addAsDependent(std::string const& modId) noexcept;
		Result<> removeAsDependent(std::string const& modId) noexcept;

		bool isDependent(std::string const& modId) noexcept;

		std::vector<std::string> getAllDependents() noexcept;

		Result<nlohmann::json*> getPermanentStorage(std::string const& modId) noexcept;

		friend class LevelImpl;
		friend class LevelPool;
	};
}