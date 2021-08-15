/*!
 * @brief	NoncopyÉ|ÉäÉVÅ[
 */

#pragma once 

namespace nsK2EngineLow {
	struct Noncopyable {
		Noncopyable() = default;
		Noncopyable(const Noncopyable&) = delete;
		Noncopyable& operator=(const Noncopyable&) = delete;
	};
}

