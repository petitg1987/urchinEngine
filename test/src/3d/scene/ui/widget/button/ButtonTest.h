#pragma once

#include <memory>
#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>
#include <Urchin3dEngine.h>

class ButtonTest final : public CppUnit::TestFixture {
    public:
        static CppUnit::Test* suite();

        void focusSwitchBetweenAdjacentButtons();
        void clickOnButtonDeactivateTextBox();
        void validateKeyConsumedOnNavigationFocus();

    private:
        std::unique_ptr<urchin::UIRenderer> setupUiRenderer();

        std::unique_ptr<urchin::OffscreenRender> renderTarget;
        std::unique_ptr<urchin::I18nService> i18nService;
};
