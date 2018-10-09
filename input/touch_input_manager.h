#ifndef _GEF_TOUCH_INPUT_MANAGER_H
#define _GEF_TOUCH_INPUT_MANAGER_H

#include <gef.h>
#include <maths/vector2.h>
#include <maths/vector4.h>
#include <vector>
#include <list>

namespace gef
{
	enum TouchType
	{
		TT_NONE = 0,
		TT_NEW,
		TT_ACTIVE,
		TT_RELEASED
	};

	struct Touch
	{
		Int32 id;
		TouchType type;
		Vector2 position;
	};

	typedef std::list<struct Touch> TouchContainer;
	typedef std::list<struct Touch>::iterator TouchIterator;
	typedef std::list<struct Touch>::const_iterator ConstTouchIterator;

	class TouchInputManager
	{
	public:
		virtual ~TouchInputManager();

		bool FindTouch(const Int32 panel_index, const Int32 touch_id, Touch& touch) const;

		inline const TouchContainer& touches(const Int32 panel_index) const
		{
			return panels_[panel_index];
		}

		virtual void Update() = 0;

		virtual void EnablePanel(const Int32 panel_index) = 0;
		virtual void DisablePanel(const Int32 panel_index) = 0;

		//virtual const Touch& GetTouch(const Int32 panel_index, const Int32 touch_index)  = 0;
//		virtual const Int32 max_num_touches() const  = 0;
		virtual const Int32 max_num_panels() const = 0;
		virtual const bool panel_enabled(const Int32 panel_index) const = 0;
		//virtual const Touch* const FindTouch(const Int32 panel_index, const Int32 touch_id) const = 0;

		//virtual const Int32 num_touches() const  = 0;

		virtual const gef::Vector2 mouse_position() const = 0;
		virtual bool is_button_down(Int32 button_num) const = 0;

		virtual const gef::Vector4 mouse_rel() const = 0;

		void CleanupReleasedTouches();
	protected:
		void AddTouch(const Int32 panel_index, const Touch& touch);
		bool RemoveTouch(const Int32 panel_index, const Int32 touch_id);
		bool UpdateTouch(const Int32 panel_index, const Touch& touch);
		
		std::vector<TouchContainer> panels_;
	};
}

#endif // _GEF_TOUCH_MANAGER_H
