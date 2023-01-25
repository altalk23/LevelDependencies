#include "LevelImpl.hpp"

#include "LevelStorage.hpp"

#include <Geode/external/json/json.hpp>

using namespace tulip::dependencies;

USE_GEODE_NAMESPACE();

Result<std::unique_ptr<Level>> LevelImpl::create(GJGameLevel* gdLevel) {
	auto level = std::make_unique<Level>();

	GEODE_UNWRAP(level->m_impl->init(gdLevel));

	return Ok(std::move(level));
}

Result<> LevelImpl::destroy(std::unique_ptr<Level>&& level) {
	GEODE_UNWRAP(level->m_impl->destroy());

	return Ok();
}

Level::Impl::Impl() noexcept {}

Level::Impl::~Impl() noexcept {}

Result<> Level::Impl::addAsDependent(std::string const& modId) noexcept {
	if (m_dependents.size() == 0) {
		LevelStorage::addKicking();
	}
	if (!this->isDependent(modId)) {
		m_dependents.insert(modId);
		m_permanentStorage.insert({modId, std::make_unique<nlohmann::json>()});
	}
	return Ok();
}

Result<> Level::Impl::removeAsDependent(std::string const& modId) noexcept {
	if (this->isDependent(modId)) {
		m_dependents.erase(modId);
		m_permanentStorage.erase(modId);
	}
	if (m_dependents.size() == 0) {
		LevelStorage::removeKicking();
	}
	return Ok();
}

bool Level::Impl::isDependent(std::string const& modId) noexcept {
	return m_dependents.count(modId) > 0;
}

std::vector<std::string> Level::Impl::getAllDependents() noexcept {
	return std::vector<std::string>(m_dependents.begin(), m_dependents.end());
}

nlohmann::json* Level::Impl::getPermanentStorage(std::string const& modId) noexcept {
	return m_permanentStorage[modId].get();
}

Result<> Level::Impl::init(GJGameLevel* level) {
	m_level = level;
	auto data = LevelStorage::retrievePermanent();
	auto json = nlohmann::json::parse(data);
	for (auto& value : json["dependents"]) {
		m_dependents.insert(value.get<std::string>());
	}
	for (auto& [key, value] : json["permanent"].items()) {
		m_permanentStorage.insert({key, std::make_unique<nlohmann::json>()});
		*m_permanentStorage[key] = nlohmann::json::parse(value.get<std::string>());
	}
	return Ok();
}

Result<> Level::Impl::destroy() {
	nlohmann::json json;
	json["dependents"] = this->getAllDependents();

	json["permanent"] = nlohmann::json::object();
	for (auto& [modId, permanent] : m_permanentStorage) {
		json["permanent"][modId] = permanent->dump();
	}
	LevelStorage::storePermanent(json.dump());
	return Ok();
}
