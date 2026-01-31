#pragma once

#include "../n_pin_part.h"


class OpAmp : public NPinPart<3> {






public:
	void pin_plus();
	void pin_minus();
	void pin_out();
};