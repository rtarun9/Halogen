#pragma once

#include "../internal/input.h"

#include "../internal/timer.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace halogen
{
	namespace camera_defaults
	{
		const glm::vec3 CAMERA_POSITION = glm::vec3(0.0f, 3.0f, -10.0f);
		constexpr float DELTA_TIME_MULTIPLIER = 0.5f;
	}

	class Camera
	{
	public:
		Camera();
		~Camera();

		Camera(Camera& other) = delete;

		void update_camera(Input& input);

		glm::vec3 m_camera_position;
		glm::vec3 m_camera_target;
		glm::vec3 m_camera_up;
		glm::vec3 m_camera_direction;
		glm::vec3 m_camera_right;
	};
}