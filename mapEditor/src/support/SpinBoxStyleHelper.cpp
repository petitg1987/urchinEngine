#include "SpinBoxStyleHelper.h"

namespace urchin
{

	void SpinBoxStyleHelper::applyNormalStyle(QDoubleSpinBox *spinBox)
	{
		spinBox->setDecimals(5);
		spinBox->setMinimum(-1e+09);
		spinBox->setMaximum(1e+09);
		spinBox->setSingleStep(0.1);
		spinBox->setKeyboardTracking(true);
		spinBox->setAccelerated(true);
		spinBox->setLocale(QLocale::English);
	}

}
