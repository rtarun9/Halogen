#pragma once

#include "vk_types.h"

struct SDL_Window;

namespace halo
{
	class Engine
	{
	public:
		Engine();

		void initialize();
		void run();
		void clean();

	private:
		void render();

	private:
		bool m_isInitialized{false};
		int m_frameNumber{0};

		int m_windowWidth{1080};
		int m_windowHeight{720};

		SDL_Window *m_window{nullptr};
		VkExtent2D m_windowExtent{(uint32_t)m_windowWidth, (uint32_t)m_windowHeight};
	};
}