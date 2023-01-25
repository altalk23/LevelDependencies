#include "LevelStorage.hpp"

#include "LevelImpl.hpp"

#include <Geode/Bindings.hpp>

using namespace tulip::dependencies;

USE_GEODE_NAMESPACE();

// any object that matches these values are for the kicking system.
static int const s_textObjectId = 914;
static CCPoint const s_textObjectPos = CCPointMake(1090, 2490);

static int const s_startPosObjectId = 31;
static CCPoint const s_startPosObjectPos = CCPointMake(1030, 5000);

static int const s_endPosObjectId = 1;
static CCPoint const s_endPosObjectPos = CCPointMake(2090, 5000);

static LevelEditorLayer* s_editorLayer = nullptr;
static PlayLayer* s_playLayer = nullptr;
static GJBaseGameLayer* s_gameLayer = nullptr;
static std::unique_ptr<Level> s_dataLevel;

void LevelStorage::addKicking() {
	if (!LevelStorage::hasKicking()) {
		auto textObj = GameObject::createWithKey(s_textObjectId);
		textObj->setPosition(s_textObjectPos);
		textObj->updateTextObject("Install Geode and Level Dependencies\nmod to play the level.", false);
		textObj->m_isDontEnter = true;
		textObj->m_isDontFade = true;
		textObj->m_scale = 0.5;
		textObj->setRScale(1.0f);
		textObj->m_zLayer = ZLayer::T3;
		s_gameLayer->addToSection(textObj);

		auto startPosObj = GameObject::createWithKey(s_startPosObjectId);
		startPosObj->setPosition(s_startPosObjectPos);
		s_gameLayer->addToSection(startPosObj);

		auto endPosObj = GameObject::createWithKey(s_endPosObjectId);
		endPosObj->setPosition(s_endPosObjectPos);
		s_gameLayer->addToSection(endPosObj);
	}
}

void LevelStorage::removeKicking() {
	if (LevelStorage::hasKicking()) {
		auto section1 = static_cast<CCArray*>(s_gameLayer->m_sectionObjects->objectAtIndex(10));

		for (auto obj : CCArrayExt<GameObject*>(section1)) {
			if (obj->getPosition() == s_startPosObjectPos) {
				s_gameLayer->removeObjectFromSection(obj);
			}

			if (obj->getPosition() == s_textObjectPos) {
				s_gameLayer->removeObjectFromSection(obj);
			}
		}

		auto section2 = static_cast<CCArray*>(s_gameLayer->m_sectionObjects->objectAtIndex(20));

		for (auto obj : CCArrayExt<GameObject*>(section2)) {
			if (obj->getPosition() == s_endPosObjectPos) {
				s_gameLayer->removeObjectFromSection(obj);
			}
		}
	}
}

bool LevelStorage::hasKicking() {
	if (s_gameLayer->m_sectionObjects->count() < 21) {
		return false;
	}
	auto section = static_cast<CCArray*>(s_gameLayer->m_sectionObjects->objectAtIndex(20));

	for (auto obj : CCArrayExt<GameObject*>(section)) {
		if (obj->getPosition() == s_endPosObjectPos) {
			return true;
		}
	}

	return false;
}

static std::string s_permamentData;
static std::string const s_permanentSignature = "  \"\"permanent~storage\"\"  ";

namespace {
	std::string getGuidelinesFromString(std::string const& combination) {
		if (combination.find(s_permanentSignature) != std::string::npos) {
			auto stream = std::stringstream(combination);

			std::string guidelines, separator, data;
			stream >> guidelines;
			if (s_permanentSignature.find(guidelines) != std::string::npos) {
				return "";
			}
			stream >> separator;
			std::getline(stream, data);

			// log::info("getg {}|{}|{}", guidelines, separator, data);

			return guidelines;
		}
		return combination;
	}

	std::string getDataFromString(std::string const& combination) {
		if (combination.find(s_permanentSignature) != std::string::npos) {
			auto stream = std::stringstream(combination);

			std::string guidelines, separator, data;
			stream >> guidelines;
			if (s_permanentSignature.find(guidelines) == std::string::npos) {
				stream >> separator;
			}
			std::getline(stream, data);

			// log::info("getd {}|{}|{}", guidelines, separator, data);

			return data;
		}

		return "";
	}

	std::string setGuidelinesToString(std::string const& combination, std::string const& guidelines) {
		// log::info("setg {}|{}|{}", guidelines, s_permanentSignature, getDataFromString(combination));
		return guidelines + s_permanentSignature + getDataFromString(combination);
	}

	std::string setDataToString(std::string const& combination, std::string const& data) {
		// log::info("setd {}|{}|{}", getGuidelinesFromString(combination), s_permanentSignature, data);
		return getGuidelinesFromString(combination) + s_permanentSignature + data;
	}

	std::string combine(std::string const& guidelines, std::string const& data) {
		return guidelines + s_permanentSignature + data;
	}
}

void LevelStorage::storePermanent(std::string const& data) {
	s_permamentData = data;
}

std::string LevelStorage::retrievePermanent() {
	return s_permamentData;
}

Level* LevelStorage::getLevel() {
	return s_dataLevel.get();
}

#include <Geode/modify/LevelEditorLayer.hpp>

struct EditorLayerData : Modify<EditorLayerData, LevelEditorLayer> {
	static void onModify(auto& self) {
		(void)self.setHookPriority("LevelEditorLayer::init", 100000);
	}

	bool init(GJGameLevel* level) {
		s_editorLayer = this;
		s_gameLayer = this;
		if (!LevelEditorLayer::init(level)) {
			return false;
		}
		auto& combination = this->m_levelSettings->m_guidelineString;
		s_permamentData = getDataFromString(combination);

		if (!s_dataLevel) {
			s_dataLevel = std::make_unique<Level>();
		}

		return true;
	}
};

#include <Geode/modify/PlayLayer.hpp>

struct PlayLayerData : Modify<PlayLayerData, PlayLayer> {
	static void onModify(auto& self) {
		(void)self.setHookPriority("PlayLayer::init", 100000);
	}

	bool init(GJGameLevel* level) {
		s_playLayer = this;
		s_gameLayer = this;
		if (!PlayLayer::init(level)) {
			return false;
		}

		auto& combination = this->m_levelSettings->m_guidelineString;
		s_permamentData = getDataFromString(combination);

		if (!s_dataLevel) {
			s_dataLevel = std::make_unique<Level>();
		}

		return true;
	}
};

#include <Geode/modify/DrawGridLayer.hpp>

struct PermanentFix : Modify<PermanentFix, DrawGridLayer> {
	void loadTimeMarkers(gd::string combination) {
		DrawGridLayer::loadTimeMarkers(getGuidelinesFromString(combination));
	}
};

#include <Geode/modify/EditorPauseLayer.hpp>

struct PermanentStorage : Modify<PermanentStorage, EditorPauseLayer> {
	void saveLevel() {
		auto& combination = s_editorLayer->m_levelSettings->m_guidelineString;
		combination = setDataToString(combination, s_permamentData);

		EditorPauseLayer::saveLevel();
	}
};

#include <Geode/modify/GameManager.hpp>

struct ExitTracker : Modify<ExitTracker, GameManager> {
	void returnToLastScene(GJGameLevel* level) {
		s_permamentData = "";
		s_editorLayer = nullptr;
		s_playLayer = nullptr;
		s_gameLayer = nullptr;
		if (s_dataLevel) {
			s_dataLevel.release();
		}
		GameManager::returnToLastScene(level);
	}
};
