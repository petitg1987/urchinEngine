#include "FrameStyleHelper.h"

namespace urchin
{

	void FrameStyleHelper::applyLineStyle(QFrame *frame)
	{
		frame->setFrameShape(QFrame::HLine);
		frame->setStyleSheet("QFrame {border: 1px solid gray;margin-top: 0.5em;}");
	}

}
