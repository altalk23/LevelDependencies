#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>

USE_GEODE_NAMESPACE();

struct DefaultHook : Modify<DefaultHook, MenuLayer> {
	void onMoreGames(CCObject*) {
		FLAlertLayer::create("Geode", "Hello from my custom mod!", "OK")->show();
	}
};
