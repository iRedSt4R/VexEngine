#include "../Windows/WinApp64.h"
#

class IApplication
{
public:
	virtual void Create(HINSTANCE hInstance, uint32_t height, uint32_t width) = 0;

	// Application Loop
	virtual void Begin() = 0;
	virtual void Update() = 0;
	virtual void End() = 0;

	virtual void Shutdown() = 0;
};
