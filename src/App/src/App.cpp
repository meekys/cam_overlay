#include <iostream>
#include <unistd.h>
#include <signal.h>

#include "App.hpp"

static volatile bool running = true;

static void interruptHandler(int /*unused*/)
{
    running = false;
}

void App::Run()
{
    _logger->Info("Dimensions: %d x %d Pixel Format: %s", _input->getWidth(), _input->getHeight(), _config->OutputFormat.c_str());

    _logger->Info("Running...");

    signal(SIGINT, interruptHandler);

    while (::running)
    {
        if (_display->isClosed())
            break;

        InputData data;
        if (!_input->Read(data))
            continue;


        _output->Update(data.data, data.size);
        
        _input->EndRead();

        ::glClear(GL_COLOR_BUFFER_BIT);
        ::glViewport(0, 0, _display->getWidth(), _display->getHeight());

        _output->Draw();
        _overlay->Draw();

        _glContext->SwapBuffers();
    }

    _logger->Info("Quitting...");
}