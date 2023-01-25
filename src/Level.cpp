#include "LevelImpl.hpp"
#include "LevelStorage.hpp"

using namespace tulip::dependencies;

USE_GEODE_NAMESPACE();

Level::Level() noexcept :
	m_impl(new Level::Impl) {}

Level::~Level() noexcept {}

Result<Level*> Level::get() noexcept {
	auto level = LevelStorage::getLevel();
	if (level) {
		return Ok(level);
	}
	return Err("no valid GJBaseGameLayer instance");
}

Result<> Level::addAsDependent(std::string const& modId) noexcept {
	return m_impl->addAsDependent(modId);
}

Result<> Level::removeAsDependent(std::string const& modId) noexcept {
	return m_impl->removeAsDependent(modId);
}

bool Level::isDependent(std::string const& modId) noexcept {
	return m_impl->isDependent(modId);
}

std::vector<std::string> Level::getAllDependents() noexcept {
	return m_impl->getAllDependents();
}

Result<nlohmann::json*> Level::getPermanentStorage(std::string const& modId) noexcept {
	if (!m_impl->isDependent(modId)) {
		return Err("Mod is not a dependent of the level");
	}
	return Ok(m_impl->getPermanentStorage(modId));
}
