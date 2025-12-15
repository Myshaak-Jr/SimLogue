#pragma once


#include "../n_pin_part.h"


class VoltageSource : public NPinPart<1> {
private:
	scalar voltage;
	size_t branch_id;

	scalar current;

public:
	explicit VoltageSource(const std::string& name, scalar voltage);
	~VoltageSource() noexcept;

	void pre_stamp(CircuitMatrix& matrix, const StampParams& params) override;
	void stamp(CircuitMatrix& matrix, const StampParams& params) const override;
	void post_stamp(const CircuitMatrix& matrix, const StampParams& params) override;

	scalar get_current_between(const ConstPin& a, const ConstPin& b) const override;
};


class VoltageSource2Pin : public NPinPart<2> {
private:
	scalar voltage;
	size_t branch_id;

	scalar current;

public:
	explicit VoltageSource2Pin(const std::string& name, scalar voltage);
	~VoltageSource2Pin() noexcept;

	void pre_stamp(CircuitMatrix& matrix, const StampParams& params) override;
	void stamp(CircuitMatrix& matrix, const StampParams& params) const override;
	void post_stamp(const CircuitMatrix& matrix, const StampParams& params) override;

	scalar get_current_between(const ConstPin& a, const ConstPin& b) const override;
};