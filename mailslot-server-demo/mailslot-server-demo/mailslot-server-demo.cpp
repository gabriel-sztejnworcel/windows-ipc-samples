
#include <Windows.h>

#include <iostream>
#include <stdexcept>
#include <string>

#define BUFFER_SIZE 4096

int main()
{
    try
    {
        wchar_t mailslot_name[] = L"\\\\.\\mailslot\\demo";
        
        HANDLE mailslot_server_handle = CreateMailslot(
            mailslot_name,
            4096,
            0,
            nullptr
        );

        if (mailslot_server_handle == INVALID_HANDLE_VALUE)
        {
            throw std::runtime_error(
                "CreateMailslot failed with error code "
                    + std::to_string(GetLastError()));
        }

        std::cout << "[+] Mailslot server created" << std::endl;

        while (true)
        {
            char buf[BUFFER_SIZE] = { 0 };
            DWORD bytes_read = 0;

            BOOL result = ReadFile(
                mailslot_server_handle,
                buf,
                BUFFER_SIZE - 1,
                &bytes_read,
                nullptr
            );

            if (!result)
            {
                auto last_error = GetLastError();

                if (last_error == ERROR_SEM_TIMEOUT)
                {
                    Sleep(200);
                    continue;
                }
                
                throw std::runtime_error(
                    "ReadFile failed with error code "
                        + std::to_string(last_error));
            }

            buf[bytes_read] = '\0';
            std::cout << "[+] Received: " << buf << std::endl;
        }
    }
    catch (const std::exception& ex)
    {
        std::cerr << "ERROR: " << ex.what() << std::endl;
    }
}
