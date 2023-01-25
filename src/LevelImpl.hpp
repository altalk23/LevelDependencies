#pragma once

#include <Level.hpp>
#include <memory>
#include <unordered_map>
#include <unordered_set>

namespace tulip::dependencies {
	class Level::Impl {
	public:
		GJGameLevel* m_level = nullptr;

		std::unordered_set<std::string> m_dependents;

		std::unordered_map<std::string, std::unique_ptr<nlohmann::json>> m_permanentStorage;

		Impl() noexcept;
		~Impl() noexcept;

		Result<> addAsDependent(std::string const& modId) noexcept;
		Result<> removeAsDependent(std::string const& modId) noexcept;

		bool isDependent(std::string const& modId) noexcept;

		std::vector<std::string> getAllDependents() noexcept;

		nlohmann::json* getPermanentStorage(std::string const& modId) noexcept;

		Result<> init(GJGameLevel* level);
		Result<> destroy();
	};

	class LevelImpl {
	public:
		static Result<std::unique_ptr<Level>> create(GJGameLevel* level);
		static Result<> destroy(std::unique_ptr<Level>&& level);
	};
}