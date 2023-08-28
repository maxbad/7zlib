#include <iostream>
#include <string>
#include <windows.h>
#include <thread>
#include "../7zlib/7zlib.h"

struct TestInfo
{
	std::thread::id id;
	std::string content;
};

void TestCallback(float percent, void* user)
{
	TestInfo* ti = static_cast<TestInfo*>(user);
	std::cout << "threadId: " << ti->id << " " << ti->content << " " << percent << "%" << std::endl;
}

void TestLib(const wchar_t* source, const wchar_t* dest, const wchar_t* out);
void TestDll();

int main()
{
#if 1
	bool result = SevenZip::Initialize();
	if (result) {
		std::thread t1(TestLib, L"", L"D:\\Dungeon Fighter\\CEA7AA6786B01A238EB42AFDD1A675CA.7z", L"D:\\Dungeon Fighter");
		t1.join();
	}
	else {
		std::cout << "initialize error!" << std::endl;
	}

	SevenZip::Uninitialize();
#else
	TestDll();
#endif

	return 0;
}

void TestLib(const wchar_t* source, const wchar_t* dest, const wchar_t* out)
{
	/*TestInfo tic;
	tic.id = std::this_thread::get_id();
	tic.content = "Compress";
	SevenZip::ResultCode resultCode1 = SevenZip::Compress(source, dest, TestCallback, &tic);
	std::cout << "threadId: " << tic.id << " compress resultCode[" << resultCode1 << "]" << std::endl;*/

	TestInfo tiuc;
	tiuc.id = std::this_thread::get_id();
	tiuc.content = "Uncompress";

	SevenZip::ResultCode resultCode2 = SevenZip::Uncompress(dest, out, TestCallback, &tiuc);
	std::cout << "threadId: " << tiuc.id << " uncompress resultCode[" << resultCode2 << "]" << std::endl;
}

void TestDll()
{
	typedef bool(*InitializeFunc)();
	typedef SevenZip::ResultCode(*CompressFunc)(const wchar_t*, const wchar_t*, SevenZip::CallbackFunc, void*);
	typedef SevenZip::ResultCode(*UncompressFunc)(const wchar_t*, const wchar_t*, SevenZip::CallbackFunc, void*);
	typedef void(*UninitializeFunc)();

	InitializeFunc initializeFunc = nullptr;
	CompressFunc compressFunc = nullptr;
	UncompressFunc uncompressFunc = nullptr;
	UninitializeFunc uninitializeFunc = nullptr;

	HMODULE hDLL = LoadLibraryW(L"7zlib.dll");
	if (hDLL != NULL)
	{
		initializeFunc = (InitializeFunc)GetProcAddress(hDLL, "Initialize");
		compressFunc = (CompressFunc)GetProcAddress(hDLL, "Compress");
		uncompressFunc = (UncompressFunc)GetProcAddress(hDLL, "Uncompress");
		uninitializeFunc = (UninitializeFunc)GetProcAddress(hDLL, "Uninitialize");

		if (initializeFunc && compressFunc && uncompressFunc && uninitializeFunc)
		{
			bool result = initializeFunc();
			if (result)
			{
				/*SevenZip::ResultCode resultCode1 = compressFunc(L"D:\\temp\\tt\\test7z", L"D:\\temp\\tt/test7z.7z", nullptr, nullptr);
				std::cout << "compress resultCode[" << resultCode1 << "]" << std::endl;*/

				SevenZip::ResultCode resultCode2 = uncompressFunc(L"D:\\Dungeon Fighter\\CEA7AA6786B01A238EB42AFDD1A675CA.7z", L"D:\\Dungeon Fighter", nullptr, nullptr);
				std::cout << "uncompress resultCode[" << resultCode2 << "]" << std::endl;
			}
			else
			{
				std::cout << "initialize error!" << std::endl;
			}
		}
		else
		{
			std::cout << "bad func address!" << std::endl;
		}

		uninitializeFunc();
	}
	else
	{
		std::cout << "can not load dll!" << std::endl;
	}
}