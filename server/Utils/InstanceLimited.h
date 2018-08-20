#pragma once

#include <cassert>

template<typename T, int limit>
class InstanceLimited {
public:
	InstanceLimited() {
		instanceCount++;
		assert(instanceCount <= limit);
	}
	~InstanceLimited() {
		instanceCount--;
	}

private:
	static int instanceCount;
};

template<typename T, int limit>
int InstanceLimited<T, limit>::instanceCount = 0;

template<typename T>
using OneInstanced = InstanceLimited<T, 1>;
