
#include <Windows.h>

#include <iostream>
#include <stdexcept>
#include <string>

std::wstring str_to_wstr(const std::string& str);

int main(int argc, char* argv[])
{
    try
    {
        if (argc != 3)
        {
            throw std::runtime_error("Invalid arguments");
        }
        
        std::wstring pipe_name = str_to_wstr(argv[1]);
        std::string msg(argv[2]);

        HANDLE pipe_client_handle = CreateFile(
            pipe_name.c_str(),
            GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ,
            nullptr,
            OPEN_EXISTING,
            0,
            nullptr
        );

        if (pipe_client_handle == INVALID_HANDLE_VALUE)
        {
            throw std::runtime_error(
                "CreateFile failed with error code " + std::to_string(GetLastError()));
        }

        std::cout << "[+] Connected to pipe server" << std::endl;

        DWORD bytes_written = 0;

        BOOL result = WriteFile(
            pipe_client_handle,
            msg.c_str(),
            msg.length(),
            &bytes_written,
            nullptr
        );

        if (!result)
        {
            throw std::runtime_error(
                "WriteFile failed with error code "
                + std::to_string(GetLastError()));
        }

        std::cout << "[+] Sent " << bytes_written << " bytes" << std::endl;
    }
    catch (const std::exception& ex)
    {
        std::cerr << "[!] ERROR: " << ex.what() << std::endl;
        exit(1);
    }

    exit(0);
}

std::wstring str_to_wstr(const std::string& str)
{
    return std::wstring(str.begin(), str.end());
}
