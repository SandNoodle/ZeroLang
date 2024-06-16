#include "common/diagnostic.h"

#include <sstream>

namespace soul
{
	diagnostic_t::operator std::string() const
	{
		std::stringstream ss;
		ss << "ERROR"; // TODO: Other types
		ss <<  std::format(" [E{:04}", static_cast<std::underlying_type_t<diagnostic_code_t>>(_code)) << "]: ";
		ss << _message << '\n';
		return ss.str();
	}

}  // namespace soul
