#include "framework.hpp"
#include "Direct2DWindow.hpp"

int __stdcall wWinMain(_In_ HINSTANCE module, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int) {
	try {
		CoInitialize(nullptr);
		Direct2DWindow window(module);
		window.run();
	}
	catch (ComException const& e) {
		std::cerr << "Error: " << std::hex << e.result << std::endl;
		return e.result;
	}
	catch (...) {
		std::cerr << "Unknown error" << std::endl;
		return E_FAIL;
	}
	CoUninitialize();
	return 0;
}