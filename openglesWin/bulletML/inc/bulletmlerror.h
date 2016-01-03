#ifndef BULLETMLERROR_H_
#define BULLETMLERROR_H_

#include "bulletmlcommon.h"

#include <stdexcept>

/// Ç≥Ç∑Ç™Ç…Ç‡Ç§ÇøÇÂÇ¡Ç∆è[é¿Ç≥ÇπÇÒÇ∆ÇÀÇ¶
class BulletMLError : public std::runtime_error {
public:
	DECLSPEC BulletMLError(const std::string& msg)
		: std::runtime_error(msg) {}

	DECLSPEC static void doAssert(const char* str) {
#ifdef WIN32
		throw BulletMLError(str);
#endif
	}
	DECLSPEC static void doAssert(const std::string& str) {
#ifdef WIN32
		throw BulletMLError(str);
#endif
	}
	DECLSPEC static void doAssert(bool t, const char* str) {
#ifdef WIN32
		if (!t) throw BulletMLError(str);
#endif
	}
	DECLSPEC static void doAssert(bool t, const std::string& str) {
#ifdef WIN32
		if (!t) throw BulletMLError(str);
#endif
	}

};

#endif // ! BULLETMLERROR_H_

