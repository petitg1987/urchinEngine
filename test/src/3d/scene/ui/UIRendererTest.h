#pragma once

#include <memory>
#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>
#include <Urchin3dEngine.h>

class UIRendererTest : public CppUnit::TestFixture {
    public:
        static CppUnit::Test* suite();

        void childRemoveItsParent();

    private:
        std::unique_ptr<urchin::UIRenderer> setupUiRenderer();

        std::unique_ptr<urchin::RenderTarget> renderTarget;
        std::unique_ptr<urchin::I18nService> i18nService;
};

