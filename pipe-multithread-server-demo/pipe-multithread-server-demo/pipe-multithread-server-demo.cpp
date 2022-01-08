
#include <Windows.h>

#include <iostream>
#include <stdexcept>
#include <string>
#include <thread>
#include <mutex>

void client_handler(HANDLE pipe_server_handle);
void output(const std::string& str, bool error = false);

int main()
{
    try
    {
        wchar_t pipe_name[] = L"\\\\.\\pipe\\demo";

        while (true)
        {
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

            output("[+] Pipe server created");

            BOOL result = ConnectNamedPipe(pipe_server_handle, nullptr);
            if (!result)
            {
                throw std::runtime_error(
                    "ConnectNamedPipe failed with error code " + std::to_string(GetLastError()));
            }

            output("[+] Client connected");

            std::thread client_thread([pipe_server_handle]()
            {
                client_handler(pipe_server_handle);
            });

            client_thread.detach();
        }
    }
    catch (const std::exception& ex)
    {
        output("[!] ERROR: " + std::string(ex.what()), true);
    }
}

void client_handler(HANDLE pipe_server_handle)
{
    try
    {
        while (true)
        {
            char buf[4096] = { 0 };
            DWORD bytes_read = 0;

            BOOL result = ReadFile(pipe_server_handle, buf, 4096, &bytes_read, nullptr);
            if (!result)
            {
                throw std::runtime_error(
                    "ReadFile failed with error code " + std::to_string(GetLastError()));
            }

            buf[bytes_read] = '\0';
            output("[+] Received: " + std::string(buf));
        }
    }
    catch (const std::exception& ex)
    {
        output("[!] ERROR: " + std::string(ex.what()), true);
    }
}

void output(const std::string& str, bool error)
{
    static std::mutex mtx;
    std::lock_guard<std::mutex> lock(mtx);

    if (error)
    {
        std::cerr << str << std::endl;
    }
    else
    {
        std::cout << str << std::endl;
    }
}
