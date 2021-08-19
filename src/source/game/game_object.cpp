#include "../../include/core/game/game_object.h"

namespace halogen
{
	GameObject::GameObject(): m_mesh(nullptr), m_id(-1)
	{
		m_material = {};
	}

	GameObject::~GameObject()
	{
		m_mesh = nullptr;
		m_material = {};
		m_id = -1;
	}

	uint32_t GameObject::create_game_object()
	{
		static uint32_t unique_id;
		m_id = ++unique_id;
		return m_id;
	}
}