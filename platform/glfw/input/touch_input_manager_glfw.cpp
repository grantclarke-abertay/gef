#include <platform/glfw/input/touch_input_manager_glfw.h>
//#include <gef.h>
#include <platform/glfw/system/platform_glfw.h>

namespace gef
{
#define INVALID_TOUCH_ID		(-1)



    static TouchInputManagerGLFW* touch_input_glfw = nullptr;

    void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
    {
        if(touch_input_glfw)
        {
            if(button >= GLFW_MOUSE_BUTTON_1 && button <= GLFW_MOUSE_BUTTON_4)
            {
                if(action == GLFW_PRESS)
                {
                    touch_input_glfw->set_live_button_down(button, true);
                }
                else
                {
                    touch_input_glfw->set_live_button_down(button, false);
                }
            }
        }
    }





    TouchInputManagerGLFW::TouchInputManagerGLFW(const PlatformGLFW* platform) :
            platform_(platform)
    {
/*
		for(Int32 panel_index=0;panel_index<1;++panel_index)
		{
			panel_enabled_[panel_index] = false;

			for(Int32 touch_index=0;touch_index<2;++touch_index)
			{
				Touch* touch = &touches_[panel_index][touch_index];
				touch->type = TT_NONE;
				touch->id = INVALID_TOUCH_ID;
				is_button_down_[touch_index] = false;
			}
		}
*/
        panels_.push_back(TouchContainer());

        is_button_down_[0] = false;
        is_button_down_[1] = false;
        is_button_down_[2] = false;
        is_button_down_[3] = false;
        is_live_button_down_[0] = false;
        is_live_button_down_[1] = false;
        is_live_button_down_[2] = false;
        is_live_button_down_[3] = false;

        double xpos, ypos;
        glfwGetCursorPos(platform_->window(), &xpos, &ypos);

        mouse_position_.x = (float)xpos;
        mouse_position_.y = (float)ypos;

        touch_input_glfw = this;
        glfwSetMouseButtonCallback(platform_->window(), mouse_button_callback);
    }

    TouchInputManagerGLFW::~TouchInputManagerGLFW()
    {
        touch_input_glfw = nullptr;
        CleanUp();
    }

    void TouchInputManagerGLFW::CleanUp()
    {

    }

    void TouchInputManagerGLFW::Update()
    {
        // store the button status for this update
        // intialise it to the same state as last frame
        bool is_button_down[4] = { false, false, false, false };

        is_button_down[0] = is_button_down_[0];
        is_button_down[1] = is_button_down_[1];
        is_button_down[2] = is_button_down_[2];
        is_button_down[3] = is_button_down_[3];


        double xpos, ypos;
        glfwGetCursorPos(platform_->window(), &xpos, &ypos);


        mouse_rel_.set_x((float)xpos - mouse_position_.x);
        mouse_rel_.set_y((float)ypos - mouse_position_.y);
        mouse_rel_.set_z(0);

        mouse_position_.x = (float)xpos;
        mouse_position_.y = (float)ypos;
        
        
        for(int button_num=0;button_num<4;button_num++)
        {
            is_button_down[button_num] = is_live_button_down_[button_num];
        }


        for(Int32 panel_index=0;panel_index<1;++panel_index)
        {
            if(panel_enabled_[panel_index])
            {
                // go through existing touches and remove any that were released last update
                for (TouchIterator touch = panels_[panel_index].begin(); touch != panels_[panel_index].end();)
                {
                    if (touch->type == TT_RELEASED)
                        touch = panels_[panel_index].erase(touch);
                    else
                        ++touch;
                }

                // get the touch data for this panel
                // check to see if we have any data to update existing touches
                for(Int32 touch_index=0;touch_index<2;++touch_index)
                {
                    Touch touch;
                    touch.id = touch_index;
                    touch.position = mouse_position_;


                    if(is_button_down[touch_index])
                    {
                        if(!is_button_down_[touch_index])
                        {
                            // NEEDS TESTING
                            // make sure mouse_position is actually within drawable area
                            if( (mouse_position_.y >= 0) && (mouse_position_.y < platform_->height()) && (mouse_position_.x >= 0) && (mouse_position_.x < platform_->width()))
                            {
                                // new touch
                                touch.type = TT_NEW;
                                AddTouch(panel_index, touch);
                            }
                        }
                        else
                        {
                            // touch updated
                            touch.type = TT_ACTIVE;
                            UpdateTouch(panel_index, touch);
                        }
                    }
                    else
                    {
                        if(is_button_down_[touch_index])
                        {
                            // touch released
                            touch.type = TT_RELEASED;
                            UpdateTouch(panel_index, touch);
                        }
                    }

                }
            }
        }

        is_button_down_[0] = is_button_down[0];
        is_button_down_[1] = is_button_down[1];
        is_button_down_[2] = is_button_down[2];
        is_button_down_[3] = is_button_down[3];

    }

    void TouchInputManagerGLFW::EnablePanel(const Int32 panel_index)
    {
        panel_enabled_[panel_index] = true;
    }

    void TouchInputManagerGLFW::DisablePanel(const Int32 panel_index)
    {
        panel_enabled_[panel_index] = false;
    }
}

