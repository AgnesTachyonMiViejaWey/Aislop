#include <Geode/Geode.hpp>
#include <Geode/modify/SecretRewardsLayer.hpp>

using namespace geode::prelude;

static int openAllTreasureRoomChests() {
	auto gsm = GameStatsManager::sharedState();
	int opened = 0;

	// Todos los cofres de la Treasure Room (1, 5, 10, 25, 50 y 100 llaves)
	// son "secret chests" numerados. Hay cientos, asi que se recorre un rango grande.
	for (int id = 1; id <= 5000; ++id) {
		if (!gsm->getRewardForSecretChest(id)) continue; // no existe ese id
		if (gsm->isSecretChestUnlocked(id)) continue;    // ya abierto

		if (auto item = gsm->unlockSecretChest(id)) {
			gsm->registerRewardsFromItem(item); // desbloquea iconos/colores/etc.
			opened++;
		}
	}

	// Guardar el archivo de guardado
	if (opened > 0) {
		AppDelegate::get()->trySaveGame(false);
	}
	return opened;
}

class $modify(OACSecretRewardsLayer, SecretRewardsLayer) {
	bool init(bool fromShop) {
		if (!SecretRewardsLayer::init(fromShop)) return false;

		auto menu = CCMenu::create();
		menu->setID("open-all-menu"_spr);

		auto spr = ButtonSprite::create("Open All", "goldFont.fnt", "GJ_button_01.png", .8f);
		auto btn = CCMenuItemSpriteExtra::create(
			spr, this, menu_selector(OACSecretRewardsLayer::onOpenAll)
		);
		menu->addChild(btn);

		auto win = CCDirector::get()->getWinSize();
		menu->setPosition({ win.width - 60.f, 25.f });
		this->addChild(menu, 100);
		return true;
	}

	void onOpenAll(CCObject*) {
		createQuickPopup(
			"Open All Chests",
			"Abrir <cy>todos</c> los cofres de la Treasure Room y guardar?",
			"Cancel", "Open",
			[](auto, bool confirmed) {
				if (!confirmed) return;
				int n = openAllTreasureRoomChests();
				FLAlertLayer::create(
					"Listo",
					n > 0
						? fmt::format("Se abrieron <cg>{}</c> cofres y se guardo tu archivo.\nSal y vuelve a entrar a la Treasure Room.", n)
						: "No quedaban cofres por abrir.",
					"OK"
				)->show();
			}
		);
	}
};
