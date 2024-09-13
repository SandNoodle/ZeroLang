#pragma once

#include <type_traits>

namespace soul
{
    class Visitor;

    /**
     * @brief Base interface class for objects that can be visited by a visitor.
     */
    class IVisitable
    {
		public:
			virtual void accept(Visitor& visitor) = 0;
			virtual void accept(Visitor& visitor) const = 0;

			friend Visitor;
    };
}  // namespace soul
