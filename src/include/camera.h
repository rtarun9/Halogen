#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace halo
{

	static constexpr float CAMERA_SPEED = 0.01f;

	struct Camera
	{
		// camera's coordinate system
		glm::vec3 m_up{ 0.0f, 1.0f, 0.0f };
		glm::vec3 m_right{ 1.0f, 0.0f, 0.0f };
		glm::vec3 m_front{ 0.0f, 0.0f, -1.0f };

		glm::vec3 m_position{ 0.0f, 0.0f, 5.0f };
		glm::vec3 m_world_up{ 0.0f, 1.0f, 0.0f };

		glm::vec3 m_target{ 0.0f, 0.0f, 0.0f };

		float m_pitch{ 0.0f };
		float m_yaw{ -90.0f };

		float m_last_x{ 0.0f };
		float m_last_y{ 0.0f };

		float m_sensitivity{ 0.02f };

		inline static Camera& get_instance()
		{
			static Camera camera;
			return camera;
		}

		inline glm::mat4 get_look_at()
		{
			return glm::lookAt(m_position, m_position + m_front, m_up);
		}

		inline void update_angles()
		{
			m_pitch = std::clamp(m_pitch, -89.0f, 89.0f);

			glm::vec3 direction{ cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch)), sin(glm::radians(m_pitch)), sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch)) };

			m_front = glm::normalize(direction);
			m_right = glm::normalize(glm::cross(m_front, m_world_up));
			m_up = glm::normalize(glm::cross(m_right, m_front));
		}

		inline void update_position(bool front, bool back, bool left, bool right, float delta_time)
		{
			if (front)
			{
				m_position += m_front * CAMERA_SPEED * delta_time;
			}
			else if (back)
			{
				m_position -= m_front * CAMERA_SPEED * (float)delta_time;
			}

			m_right = glm::normalize(glm::cross(m_front, m_up));

			if (left)
			{
				m_position -= m_right * CAMERA_SPEED * delta_time;
			}
			else if (right)
			{
				m_position += m_right * CAMERA_SPEED * delta_time;
			}

		}
	};
}