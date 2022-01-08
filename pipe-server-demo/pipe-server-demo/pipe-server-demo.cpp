
#include <Windows.h>

#include <iostream>
#include <stdexcept>
#include <string>

int main()
{
    try
    {
        wchar_t pipe_name[] = L"\\\\.\\pipe\\demo";

        HANDLE pipe_server_handle = CreateNamedPipe(
            pipe_name,
            PIPE_ACCESS_DUPLEX,
            PIPE_TYPE_BYTE | PIPE_READMODE_BYTE,
            PIPE_UNLIMITED_INSTANCES,
            4096,
            4096,
            NMPWAIT_USE_DEFAULT_WAIT,
            nullptr
        );

        if (pipe_server_handle == INVALID_HANDLE_VALUE)
        {
            throw std::runtime_error(
                "CreateNamedPipe failed with error code " + std::to_string(GetLastError()));
        }

        std::cout << "[+] Pipe server created" << std::endl;

        BOOL result = ConnectNamedPipe(pipe_server_handle, nullptr);
        if (!result)
        {
            throw std::runtime_error(
                "ConnectNamedPipe failed with error code " + std::to_string(GetLastError()));
        }

        std::cout << "[+] Client connected" << std::endl;

        while (true)
        {
            char buf[4096] = { 0 };
            DWORD bytes_read = 0;

            result = ReadFile(pipe_server_handle, buf, 4096, &bytes_read, nullptr);
            if (!result)
            {
                throw std::runtime_error(
                    "ReadFile failed with error code " + std::to_string(GetLastError()));
            }

            buf[bytes_read] = '\0';
            std::cout << "[+] Received: " << buf << std::endl;
        }
    }
    catch (const std::exception& ex)
    {
        std::cerr << "[!] ERROR: " << ex.what() << std::endl;
    }
}
