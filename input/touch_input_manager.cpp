#include <input/touch_input_manager.h>

namespace gef
{
	TouchInputManager::~TouchInputManager()
	{
/*
		for(std::vector<TouchContainer>::iterator panel_iter = panels_.begin(); panel_iter != panels_.end(); ++panel_iter)
		{
			for(TouchIterator touch_iter = panel_iter->begin(); touch_iter != panel_iter->end(); ++touch_iter)
				delete touch_iter;
		}
*/
	}

	void TouchInputManager::AddTouch(const Int32 panel_index, const Touch& touch)
	{
		panels_[panel_index].push_back(touch);
	}

	bool TouchInputManager::RemoveTouch(const Int32 panel_index, const Int32 touch_id)
	{
		bool success = false;
		for(TouchIterator touch_iter = panels_[panel_index].begin(); touch_iter != panels_[panel_index].end(); ++touch_iter)
		{
			if(touch_iter->id == touch_id)
			{
				panels_[panel_index].erase(touch_iter);
				success = true;
				break;
			}
		}
		return success;
	}

	bool TouchInputManager::UpdateTouch(const Int32 panel_index, const Touch& touch)
	{
		bool success = false;
		for(TouchIterator touch_iter = panels_[panel_index].begin(); touch_iter != panels_[panel_index].end(); ++touch_iter)
		{
			if(touch_iter->id == touch.id)
			{
				*touch_iter = touch;
				success = true;
				break;
			}
		}
		return success;
	}

	bool TouchInputManager::FindTouch(const Int32 panel_index, const Int32 touch_id, Touch& touch) const
	{
		bool found = false;
		for(ConstTouchIterator touch_iter = panels_[panel_index].begin(); touch_iter != panels_[panel_index].end(); ++touch_iter)
		{
			if(touch_iter->id == touch_id)
			{
				touch = *touch_iter;
				found = true;
				break;
			}
		}

		return found;
	}

	void TouchInputManager::CleanupReleasedTouches()
	{
		// remove any touches that were released in the previous update
		for(std::vector<TouchContainer>::iterator panel_iter = panels_.begin(); panel_iter != panels_.end(); ++panel_iter)
		{
			for(TouchIterator touch_iter = panel_iter->begin(); touch_iter != panel_iter->end();)
			{
				if(touch_iter->type == TT_RELEASED)
					touch_iter = panel_iter->erase(touch_iter);
				else
					++touch_iter;
			}
		}
	}
}