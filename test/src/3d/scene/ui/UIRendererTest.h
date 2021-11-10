#pragma once

#include <memory>
#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>
#include <Urchin3dEngine.h>

class UIRendererTest : public CppUnit::TestFixture {
    public:
        static CppUnit::Test* suite();

        void focusState();
        void noFocusStateBecauseOutsideContainer();
        void clickingState();
        void noClickingStateBecauseMouseOutside();
        void focusLostEvent();
        void focusLostEventWithClick();

        void parentPixelPosition();
        void relativeParentPixelPosition();
        void screenPercentagePosition();
        void containerPercentagePosition();
        void relativeLengthSize();

        void buttonRemoveParentContainer();

        void containerWithLazyWidgets();

    private:
        std::unique_ptr<urchin::UIRenderer> setupUiRenderer();

        std::unique_ptr<urchin::RenderTarget> renderTarget;
        std::unique_ptr<urchin::I18nService> i18nService;
};

