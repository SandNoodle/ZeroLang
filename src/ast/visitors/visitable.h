#pragma once

#include <type_traits>

namespace soul
{
    class visitor_t;

    /**
     * @brief Base interface class for objects that can be visited by a vistor.
     */
    class visitable_t
    {
		public:
			virtual void accept(visitor_t& visitor) = 0;
			virtual void accept(visitor_t& visitor) const = 0;

			friend visitor_t;
    };
}  // namespace soul
